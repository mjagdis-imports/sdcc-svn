// (c) 2023 Philipp Klaus Krause, philipp@colecovision.eu
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//
// Generalized constant propagation.

#undef DEBUG_GCP_ANALYSIS
#define DEBUG_GCP_OPT

#include <map>
#include <set>
#include <queue>
#include <iostream>

// Workaround for boost bug #11880
#include <boost/version.hpp>
#if BOOST_VERSION == 106000
   #include <boost/type_traits/ice.hpp>
#endif

#include <boost/graph/graphviz.hpp>

#include "SDCCtree_dec.hpp" // We just need it for the titlewriter for debug cfg dumping.

extern "C"
{
#include "common.h"
}

static bool
operator != (const valinfo &v0, const valinfo &v1)
{
  if (v0.nothing && v1.nothing)
    return (true);
  return (v0.nothing != v1.nothing || v0.anything != v1.anything || v0.min != v1.min || v0.max != v1.max);
}

struct valinfos
{
  std::map <int, struct valinfo> map;
};

struct cfg_genconstprop_node
{
  iCode *ic;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, cfg_genconstprop_node, struct valinfos> cfg_t;

// A quick-and-dirty function to get the CFG from sdcc (a simplified version of the function from SDCCralloc.hpp).
static void
create_cfg_genconstprop (cfg_t &cfg, iCode *start_ic, ebbIndex *ebbi)
{
  iCode *ic;

  std::map<int, unsigned int> key_to_index;
  {
    int i;

    for (ic = start_ic, i = 0; ic; ic = ic->next, i++)
      {
        boost::add_vertex(cfg);
        key_to_index[ic->key] = i;
        cfg[i].ic = ic;
      }
  }

  // Get control flow graph from sdcc.
  for (ic = start_ic; ic; ic = ic->next)
    {
      if (ic->op != GOTO && ic->op != RETURN && ic->op != JUMPTABLE && ic->next)
        boost::add_edge(key_to_index[ic->key], key_to_index[ic->next->key], cfg);

      if (ic->op == GOTO)
        boost::add_edge(key_to_index[ic->key], key_to_index[eBBWithEntryLabel(ebbi, ic->label)->sch->key], cfg);
      else if (ic->op == RETURN)
        boost::add_edge(key_to_index[ic->key], key_to_index[eBBWithEntryLabel(ebbi, returnLabel)->sch->key], cfg);
      else if (ic->op == IFX)
        boost::add_edge(key_to_index[ic->key], key_to_index[eBBWithEntryLabel(ebbi, IC_TRUE(ic) ? IC_TRUE(ic) : IC_FALSE(ic))->sch->key], cfg);
      else if (ic->op == JUMPTABLE) // This can create a multigraph. We actually need those multiple edges later for correctness of the analysis.
        for (symbol *lbl = (symbol *)(setFirstItem (IC_JTLABELS (ic))); lbl; lbl = (symbol *)(setNextItem (IC_JTLABELS (ic))))
          boost::add_edge(key_to_index[ic->key], key_to_index[eBBWithEntryLabel(ebbi, lbl)->sch->key], cfg);
    }
}

struct valinfo
getTypeValinfo (sym_link *type)
{
  struct valinfo v;
  v.anything = true;
  v.nothing = false;
  v.min = v.max = 0;

  if (IS_BOOLEAN (type))
    {
      v.anything = false;
      v.min = 0;
      v.max = 1;
    }
  else if (IS_INTEGRAL (type) && IS_UNSIGNED (type) && bitsForType (type) < 64)
    {
      v.anything = false;
      v.min = 0;
      v.max = 0xffffffffffffffffull >> (64 - bitsForType (type));
    }
  else if (IS_INTEGRAL (type) && !IS_UNSIGNED (type) && bitsForType (type) < 63)
    {
      v.anything = false;
      v.max = 0x7fffffffffffffffull >> (64 - bitsForType (type));
      v.min = -v.max - 1;
    }
  return (v);
}

struct valinfo
getOperandValinfo (const iCode *ic, const operand *op)
{
  wassert (ic);

  struct valinfo v;
  v.anything = true;
  v.nothing = false;
  v.min = v.max = 0;

  if (!op)
    return (v);

  sym_link *type = operandType (op);

  if (IS_OP_LITERAL (op) && IS_INTEGRAL (type) && bitsForType (type) < 64) // Todo: More exact check than this bits thing?
    {
      long long litval = operandLitValueUll (op);
      v.anything = false;
      v.min = litval;
      v.max = litval;
      return (v);
    }
  else if (IS_ITEMP (op))
    {
      if (ic->valinfos && ic->valinfos->map.find (op->key) != ic->valinfos->map.end ())
        return (ic->valinfos->map[op->key]);
      v.nothing = true;
      v.anything = false;
      return (v);
    }
  else
    return (getTypeValinfo (type));
}

static bool
valinfo_union (struct valinfo *v0, const struct valinfo &v1)
{
  bool change = false;
  auto new_anything = v0->anything || v1.anything;
  change |= (v0->anything != new_anything);
  v0->anything = new_anything;
  auto new_nothing = v0->nothing && v1.nothing;
  change |= (v0->nothing != new_nothing);
  v0->nothing = new_nothing;
  auto new_min = std::min (v0->min, v1.min);
  change |= (v0->min != new_min);
  v0->min = new_min;
  auto new_max = std::max (v0->max, v1.max);
  change |= (v0->max != new_max);
  v0->max = new_max;
  return (change);
}

bool
valinfos_union (iCode *ic, int key, const struct valinfo &v)
{
  if (!ic /*|| !bitVectBitValue(ic->rlive, key)*/) // Unfortunately, rlive info is inaccurate, so we can't rely on it.
    return (false);

  if (ic->valinfos && ic->valinfos->map.find (key) != ic->valinfos->map.end())
    return (valinfo_union (&ic->valinfos->map[key], v));
  else
    {
      if (!ic->valinfos)
        ic->valinfos = new struct valinfos;
      ic->valinfos->map[key] = v;
      return (true);
    }
}

bool
valinfos_unions (iCode *ic, const struct valinfos &v)
{
  bool change = false;
  for (auto i = v.map.begin(); i != v.map.end(); ++i)
    change |= valinfos_union (ic, i->first, i->second);
  return (change);
}

static void
dump_op_info (std::ostream &os, const iCode *ic, operand *op)
{
  struct valinfo v = getOperandValinfo (ic, op);
  os << "";
  if (v.nothing)
    os << "X";
  if (v.anything)
    os << "*";
  else
    os << "[" << v.min << ", " << v.max << "]";
}

// Dump cfg.
static void
dump_cfg_genconstprop (const cfg_t &cfg)
{
  std::ofstream dump_file ((std::string (dstFileName) + ".dumpgenconstpropcfg" + (currFunc ? currFunc->rname : "__global") + ".dot").c_str());

  std::string *name = new std::string[num_vertices (cfg)];
  for (unsigned int i = 0; i < boost::num_vertices (cfg); i++)
    {
      std::ostringstream os;
      iCode *ic = cfg[i].ic;
      os << i << ", " << ic->key << ": (";
      if (ic->left)
        dump_op_info (os, ic, IC_LEFT (ic));
      os << ", ";
      if (ic->right)
        dump_op_info (os, ic, IC_RIGHT (ic));
      os << ")";
      if (ic->resultvalinfo)
        {
          os << " -> ";
          if (ic->resultvalinfo->nothing)
            os << "X";
          else if (ic->resultvalinfo->anything)
            os << "*";
          else
            os << "[" << ic->resultvalinfo->min << ", " << ic->resultvalinfo->max << "]";
        }
      name[i] = os.str();
    }
  boost::write_graphviz(dump_file, cfg, boost::make_label_writer(name), boost::default_writer(), cfg_titlewriter(currFunc->rname, " generalized constant propagation"));
  delete[] name;
}

static void
valinfoPlus (struct valinfo *result, const struct valinfo &left, const struct valinfo &right)
{
  // todo: rewrite using ckd_add when we can assume host compiler has c2x support!
  if (!left.anything && !right.anything &&
    left.min > LLONG_MIN / 2 && right.min > LLONG_MIN / 2 &&
    left.max < LLONG_MAX / 2 && right.max < LLONG_MAX / 2)
    {
      result->nothing = left.nothing || right.nothing;
      auto min = left.min + right.min;
      auto max = left.max + right.max;
      if (result->anything || min >= result->min && max <= result->max)
        {
          result->anything = false;
          result->min = min;
          result->max = max;
        }
    }
}

static void
valinfoMinus (struct valinfo *result, const struct valinfo &left, const struct valinfo &right)
{
  // todo: rewrite using ckd_sub when we can assume host compiler has c2x support!
  if (!left.anything && !right.anything &&
    left.min > LLONG_MIN / 2 && right.min > LLONG_MIN / 2 &&
    left.max < LLONG_MAX / 2 && right.max < LLONG_MAX / 2)
    {
      result->nothing = left.nothing || right.nothing;
      auto min = left.min - right.max;
      auto max = left.max - right.min;
      if (result->anything || min >= result->min && max <= result->max)
        {
          result->anything = false;
          result->min = min;
          result->max = max;
        }
    }
}

static void
valinfoOr (struct valinfo *result, const struct valinfo &left, const struct valinfo &right)
{
  if (!left.anything && !right.anything &&
    left.min >= 0 && right.min >= 0 && !result->anything)
    {
      result->nothing = left.nothing || right.nothing;
      result->min = std::max (left.max, right.max);
    }
}

static void
valinfoAnd (struct valinfo *result, const struct valinfo &left, const struct valinfo &right)
{
  if (!left.anything && !right.anything &&
    left.min >= 0 && right.min >= 0)
    {
      result->anything = false;
      result->nothing = left.nothing || right.nothing;
      result->min = 0;
      auto max = std::min(left.max, right.max);
      if (max <= result->max)
        result->max = max;
    }
}

static void
valinfoRight (struct valinfo *result, const struct valinfo &left, const struct valinfo &right)
{
  if (!left.anything && !right.anything &&
    left.min >= 0 && right.min >= 0 && right.min <= 60)
    {
      result->anything = false;
      result->nothing = left.nothing || right.nothing;
      result->min = 0;
      auto max = (left.max >> right.min);
      if (max <= result->max)
        result->max = max;
    }
}

static void
valinfoCast (struct valinfo *result, sym_link *targettype, const struct valinfo &right)
{
  *result = getTypeValinfo (targettype);
  if (right.nothing)
    result->nothing = true;
  else if (!right.anything && IS_INTEGRAL (targettype) && 
    (!result->anything && right.min >= result->min && right.max <= result->max || result->anything))
    {
      result->anything = false;
      result->min = right.min;
      result->max = right.max;
    }
}

#define PASS_LIMIT = 4;

static void
recompute_node (cfg_t &G, unsigned int i, ebbIndex *ebbi, std::pair<std::queue<unsigned int>, std::set<unsigned int> > &todo, bool externchange, bool end_it_quickly)
{
  iCode *const ic = G[i].ic;
  bool change = externchange;

  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);
  struct valinfo oldleftvalinfo = getOperandValinfo (ic, left);
  struct valinfo oldrightvalinfo = getOperandValinfo (ic, right);

  if (!ic->valinfos)
    ic->valinfos = new struct valinfos;

  // Gather incoming information.
  typedef /*typename*/ boost::graph_traits<cfg_t>::in_edge_iterator in_iter_t;
  in_iter_t in, in_end;
  for (boost::tie(in, in_end) = boost::in_edges(i, G); in != in_end; ++in)
    change |= valinfos_unions (ic, G[*in]);

  typedef /*typename*/ boost::graph_traits<cfg_t>::out_edge_iterator out_iter_t;
  out_iter_t out, out_end;
  boost::tie(out, out_end) = boost::out_edges(i, G);

  if (!change || out == out_end)
    return;

  symbol *resultsym;
  if (IC_RESULT (ic) && IS_SYMOP (IC_RESULT (ic)) && !POINTER_SET (ic))
    resultsym = OP_SYMBOL (IC_RESULT (ic));
  else
    resultsym = 0;
  struct valinfo resultvalinfo;

  struct valinfo leftvalinfo = getOperandValinfo (ic, left);
  struct valinfo rightvalinfo = getOperandValinfo (ic, right);

  bool localchange = externchange || leftvalinfo != oldleftvalinfo || rightvalinfo != oldrightvalinfo;

  switch (ic->op)
    {
    case IFX:
    case JUMPTABLE:
      for(; out != out_end; ++out)
        {
          G[*out] = *ic->valinfos;
          if (todo.second.find (boost::target(*out, G)) == todo.second.end())
            {
              todo.first.push (boost::target(*out, G));
              todo.second.insert (boost::target(*out, G));
            }
        }
      if (ic->op == IFX && bitVectnBitsOn (OP_DEFS (ic->left)) == 1) // Propagate some info on the condition into the branches.
        {
          iCode *cic = (iCode *)hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_DEFS (ic->left)));
          struct valinfo v = getOperandValinfo (ic, cic->left);
          if (cic->op == '>' && IS_ITEMP (cic->left) && IS_INTEGRAL (operandType (cic->left)) &&
            IS_OP_LITERAL (IC_RIGHT (cic)) && !v.anything && !v.nothing && operandLitValueUll(IC_RIGHT (cic)) < +1000)
            {
              long long litval = operandLitValueUll (IC_RIGHT (cic));
              struct valinfo v_true = v;
              struct valinfo v_false = v;
              v_true.min = std::max (v.min, litval + 1);
              v_false.max = std::min (v.max, litval);
              int key_true = IC_TRUE (ic) ? eBBWithEntryLabel(ebbi, IC_TRUE(ic))->sch->key : ic->next->key;
              int key_false = IC_FALSE (ic) ? eBBWithEntryLabel(ebbi, IC_FALSE(ic))->sch->key : ic->next->key;
              boost::tie(out, out_end) = boost::out_edges(i, G);
              for(; out != out_end; ++out)
                if (G[boost::target(*out, G)].ic->key == key_true)
                  G[*out].map[cic->left->key] = v_true;
                else if (G[boost::target(*out, G)].ic->key == key_false)
                  G[*out].map[cic->left->key] = v_false;
            }
          if (cic->op == '<' && IS_ITEMP (cic->left) && IS_INTEGRAL (operandType (cic->left)) &&
            IS_OP_LITERAL (IC_RIGHT (cic)) && !v.anything && !v.nothing && operandLitValueUll(IC_RIGHT (cic)) < 0xffffffff)
            {
              long long litval = operandLitValueUll (IC_RIGHT (cic));
              struct valinfo v_true = v;
              struct valinfo v_false = v;
              v_true.max = std::min (v.max, litval - 1);
              v_false.min = std::max (v.min, litval);
              int key_true = IC_TRUE (ic) ? eBBWithEntryLabel(ebbi, IC_TRUE(ic))->sch->key : ic->next->key;
              int key_false = IC_FALSE (ic) ? eBBWithEntryLabel(ebbi, IC_FALSE(ic))->sch->key : ic->next->key;
              boost::tie(out, out_end) = boost::out_edges(i, G);
              for(; out != out_end; ++out)
                if (G[boost::target(*out, G)].ic->key == key_true)
                  G[*out].map[cic->left->key] = v_true;
                else if (G[boost::target(*out, G)].ic->key == key_false)
                  G[*out].map[cic->left->key] = v_false;
            }
        }
      break;
    default:
      G[*out] = *ic->valinfos;

      if (resultsym)
        resultvalinfo = getTypeValinfo (operandType (IC_RESULT (ic)));

#ifdef DEBUG_GCP_ANALYSIS
      if (localchange)
        std::cout << "Recompute node " << i << " ic " << ic->key << "\n";
#endif
// todo: handle more operations: |, ^.
      if (!localchange) // Input didn't change. No need to recompute result.
        resultsym = 0;
      else if (end_it_quickly) // Just use the very rough approximation from the type info only to speed up analysis.
        ;
      else if (ic->op == '!')
        {
          resultvalinfo.nothing = leftvalinfo.nothing;
          resultvalinfo.anything = false;
          resultvalinfo.min = 0;
          resultvalinfo.max = 1;
          if (!leftvalinfo.anything && (leftvalinfo.min > 0 || leftvalinfo.max < 0))
            resultvalinfo.max = 0;
          else if (!leftvalinfo.anything && leftvalinfo.min == 0 && leftvalinfo.max == 0)
            resultvalinfo.min = 1;
        }
      else if (ic-> op == UNARYMINUS)
        {
          struct valinfo z;
          z.nothing = false;
          z.anything = false;
          z.min = z.max = 0;
          valinfoMinus (&resultvalinfo, z, leftvalinfo);
        }
      else if (ic->op == '<' || ic->op == GE_OP)
        {
          resultvalinfo.nothing = leftvalinfo.nothing || rightvalinfo.nothing;
          resultvalinfo.anything = false;
          resultvalinfo.min = 0;
          resultvalinfo.max = 1;
          if (!leftvalinfo.anything && !rightvalinfo.anything)
            {
              if (leftvalinfo.max < rightvalinfo.min)
                resultvalinfo.min = resultvalinfo.max = (ic->op == '<');
              else if (leftvalinfo.min >= rightvalinfo.max)
                resultvalinfo.min = resultvalinfo.max = (ic->op == GE_OP);
            }
        }
      else if (ic->op == '>' || ic->op == LE_OP)
        {
          resultvalinfo.nothing = leftvalinfo.nothing || rightvalinfo.nothing;
          resultvalinfo.anything = false;
          resultvalinfo.min = 0;
          resultvalinfo.max = 1;
          if (!leftvalinfo.anything && !rightvalinfo.anything)
            {
              if (leftvalinfo.min > rightvalinfo.max)
                resultvalinfo.min = resultvalinfo.max = (ic->op == '>');
              else if (leftvalinfo.max <= rightvalinfo.min)
                resultvalinfo.min = resultvalinfo.max = (ic->op == LE_OP);
            }
        }
      else if (ic->op == NE_OP || ic->op == EQ_OP)
        {
          resultvalinfo.nothing = leftvalinfo.nothing || rightvalinfo.nothing;
          resultvalinfo.anything = false;
          resultvalinfo.min = 0;
          resultvalinfo.max = 1;
          if (!leftvalinfo.anything && !rightvalinfo.anything && leftvalinfo.min == leftvalinfo.max && rightvalinfo.min == rightvalinfo.max)
            {
              bool one = (leftvalinfo.min == rightvalinfo.min) ^ (ic->op == NE_OP);
              resultvalinfo.min = one;
              resultvalinfo.max = one;
            }
        }
      else if (ic->op == '+')
        valinfoPlus (&resultvalinfo, leftvalinfo, rightvalinfo);
      else if (ic->op == '-')
        valinfoMinus (&resultvalinfo, leftvalinfo, rightvalinfo);
      else if (ic->op == '|')
        valinfoOr (&resultvalinfo, leftvalinfo, rightvalinfo);
      else if (ic->op == BITWISEAND)
        valinfoAnd (&resultvalinfo, leftvalinfo, rightvalinfo);
      else if (ic->op == RIGHT_OP)
        valinfoRight (&resultvalinfo, leftvalinfo, rightvalinfo);
      else if (ic->op == '=' && !POINTER_SET (ic) || ic->op == CAST)
        //resultvalinfo = rightvalinfo; // Doesn't work for = - sometimes = with mismatched types arrive here.
        valinfoCast (&resultvalinfo, operandType (IC_RESULT (ic)), rightvalinfo);

      if (resultsym)
        {
          if (!ic->resultvalinfo)
            ic->resultvalinfo = new struct valinfo;
          *ic->resultvalinfo = resultvalinfo;
          G[*out].map[resultsym->key] = resultvalinfo;
        }
      if (todo.second.find (boost::target(*out, G)) == todo.second.end())
        {
          todo.first.push (boost::target(*out, G));
          todo.second.insert (boost::target(*out, G));
        }
    }
}

// Calculate valinfos for all iCodes in function.
void
recomputeValinfos (iCode *sic, ebbIndex *ebbi)
{
  unsigned int max_rounds = 1000; // Rapidly end analysis once this number of rounds has been exceeded.

  cfg_t G;

  create_cfg_genconstprop(G, sic, ebbi);

  std::pair <std::queue<unsigned int>, std::set<unsigned int> > todo; // Nodes where valinfos need to be updated. We need a pair of a queue and a set to implement a queue with uniqe entries. A plain set wouldn't work, as we'd be working on some nodes all the time while never getting to others before we reach the round limit.

  // Process each node at least once.
  for (unsigned int i = 0; i < boost::num_vertices (G); i++)
    {
      delete G[i].ic->valinfos;
      G[i].ic->valinfos = NULL;
      delete G[i].ic->resultvalinfo;
      G[i].ic->resultvalinfo = NULL;
      recompute_node (G, i, ebbi, todo, true, false);
    }

  // Forward pass to get first approximation.
  for (unsigned int round = 0; !todo.first.empty (); round++)
    {
      // Take next node that needs updating.
      unsigned int i = todo.first.front ();
      todo.first.pop ();
      todo.second.erase (i);

#ifdef DEBUG_GCP_ANALYSIS
      std::cout << "Round " << round << " node " << i << " ic " << G[i].ic->key << "\n";
#endif
      recompute_node (G, i, ebbi, todo, false, round >= max_rounds);
    }

  // Refinement backward pass.
  // TODO

  if(options.dump_graphs)
    dump_cfg_genconstprop(G);
}

// Try to replace operands by constants
static void
optimizeValinfoConst (iCode *sic)
{
#ifdef DEBUG_GCP_OPT
  std::cout << "optimizeValifoConst at " << (currFunc ? currFunc->name : "[NOFUNC]") << "\n";
#endif
  for (iCode *ic = sic; ic; ic = ic->next)
    {
      if (SKIP_IC2 (ic))
        ;
      else
        {
          operand *left = IC_LEFT (ic);
          operand *right = IC_RIGHT (ic);
          operand *result = IC_RESULT (ic);
          const valinfo vleft = getOperandValinfo (ic, left);
          const valinfo vright = getOperandValinfo (ic, right);
          if (ic->resultvalinfo && !ic->resultvalinfo->anything && ic->resultvalinfo->min == ic->resultvalinfo->max &&
            !(ic->op == '=' && IS_OP_LITERAL (right)))
            {
              const valinfo &vresult = *ic->resultvalinfo;
#ifdef DEBUG_GCP_OPT
              std::cout << "Replace result at " << ic->key << ". anything " << vresult.anything << " min " << vresult.min << " max " << vresult.max << "\n";
#endif
              if (IS_SYMOP (left))
                bitVectUnSetBit (OP_USES (left), ic->key);
              if (IS_SYMOP (right))
                bitVectUnSetBit (OP_USES (right), ic->key);
              ic->op = '=';
              ic->left = NULL;
              ic->right = operandFromValue (valCastLiteral (operandType (result), vresult.min, vresult.min), false);
            }
          else
            {
              if (left && IS_ITEMP (left) && !vleft.anything && vleft.min == vleft.max)
                {
#ifdef DEBUG_GCP_OPT
                  std::cout << "Replace left (" << OP_SYMBOL(left)->name << "), key " << left->key << " at " << ic->key << "\n";std::cout << "anything " << vleft.anything << " min " << vleft.min << " max " << vleft.max << "\n";
#endif
                  bitVectUnSetBit (OP_USES (left), ic->key);
                  ic->left = operandFromValue (valCastLiteral (operandType (left), vleft.min, vleft.min), false);
                }
              if (right && IS_ITEMP (right) && !vright.anything && vright.min == vright.max)
                {
#ifdef DEBUG_GCP_OPT
                  std::cout << "Replace right at " << ic->key << "\n";std::cout << "anything " << vleft.anything << " min " << vleft.min << " max " << vleft.max << "\n";
#endif
                  bitVectUnSetBit (OP_USES (right), ic->key);
                  ic->right = operandFromValue (valCastLiteral (operandType (right), vright.min, vright.min), false);
                }
            }
          
        }
    }
}

static void
optimizeNarrowOpCandidate (struct valinfo *v, operand *op)
{
  wassert (v && op);
  if (!IS_INTEGRAL (operandType (op)))
    v->anything = true;
  else if (IS_OP_LITERAL (op))
    {
      long long litval = operandLitValueUll (op);
      v->anything = litval < -100000 || litval > 100000;
      v->min = litval;
      v->max = litval;
    }
  else if (bitVectnBitsOn (OP_USES (op)) != 1)
    v->anything = true;
  else
    {
      bitVect *defs = bitVectCopy (OP_DEFS (op));
      for (int key = bitVectFirstBit (defs); bitVectnBitsOn (defs); bitVectUnSetBit (defs, key), key = bitVectFirstBit (defs))
        {
          iCode *dic = (iCode *)hTabItemWithKey (iCodehTab, key);
          wassert (dic && dic->resultvalinfo);
          if (dic->op != CAST && !(dic->op == '=' && !POINTER_SET (dic)))
            {
              v->anything = true;
              return;
            }
          valinfo_union (v, *dic->resultvalinfo);
        }
      freeBitVect (defs);
    }
}

static void
optimizeNarrowResultCandidate (struct valinfo *v, operand *op)
{
  wassert (v && op);
  if (!IS_INTEGRAL (operandType (op)))
    v->anything = true;
  else if (bitVectnBitsOn (OP_DEFS (op)) != 1)
    v->anything = true;
  else
    {
      bitVect *uses = bitVectCopy (OP_USES (op));
      for (int key = bitVectFirstBit (uses); bitVectnBitsOn (uses); bitVectUnSetBit (uses, key), key = bitVectFirstBit (uses))
        {
          iCode *uic = (iCode *)hTabItemWithKey (iCodehTab, key);
          if (!uic)
            bitVectUnSetBit (OP_USES (op), key); // Looks like some earlier optimization didn't clean up properly. Do it now.
          else if (uic->op == CAST)
            ;
          else if (uic->op == EQ_OP || uic->op == NE_OP ||
            uic->op == '<' || uic->op == LE_OP || uic->op == '>' || uic->op == GE_OP)
            {
              const operand *otherop = isOperandEqual (op, uic->left) ? uic->right: uic->left;
              valinfo_union (v, getOperandValinfo (uic, otherop));
            }
          else
            {
              v->anything = true;
              break;
            }
        }
      freeBitVect (uses);
    }
}

static void
reTypeOp (operand *op, sym_link *newtype)
{
  if (IS_OP_LITERAL (op))
    {
      op->svt.valOperand = valCastLiteral (newtype, operandLitValue (op), operandLitValueUll (op));
      return;
    }

  // Replace at uses.
  bitVect *uses = bitVectCopy (OP_USES (op));
  for (int key = bitVectFirstBit (uses); bitVectnBitsOn (uses); bitVectUnSetBit (uses, key), key = bitVectFirstBit (uses))
    {std::cout << "Replace def at " << key << "\n";
      iCode *uic = (iCode *)hTabItemWithKey (iCodehTab, key);
      wassert (uic);
      if (isOperandEqual (op, uic->left))
        setOperandType (uic->left, newtype);
      if (isOperandEqual (op, uic->right))
        setOperandType (uic->right, newtype);
    }
  freeBitVect (uses);

  // Replace at definitions.
  bitVect *defs = bitVectCopy (OP_DEFS (op));
  for (int key = bitVectFirstBit (defs); bitVectnBitsOn (defs); bitVectUnSetBit (defs, key), key = bitVectFirstBit (defs))
    {
      iCode *dic = (iCode *)hTabItemWithKey (iCodehTab, key);
      wassert (dic && dic->result && isOperandEqual (op, dic->result));
      setOperandType (dic->result, newtype);
      if (dic->op == '=' && IS_OP_LITERAL (dic->right))
        reTypeOp (dic->right, newtype);
      else if (dic->op == CAST || dic->op == '=')
        {
          dic->op = CAST;
          dic->left = operandFromLink (newtype);
        }
    }
  freeBitVect (defs);
}

// todo: Remove this, use stdc_bit_width instead once we can assume C2X support on host compiler
unsigned int my_stdc_bit_width (unsigned long long value)
{
  unsigned int width = 0;
  for (int i = 0; i < ULLONG_WIDTH; i++)
    if (value & (1ull << i))
      width = (i + 1);
  return width;
}

static void
optimizeBinaryOpWithoutResult (iCode *ic)
{
  // todo
}

static void
optimizeBinaryOpWithResult (iCode *ic)
{
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);
  operand *result = IC_RESULT (ic);

  if (!IS_INTEGRAL (operandType (result)) || !IS_ITEMP (result) || bitVectnBitsOn (OP_DEFS (result)) != 1 || !ic->resultvalinfo)
    return;

  if (!IS_ITEMP (left) && !IS_OP_LITERAL (left) || !IS_ITEMP (right) && !IS_OP_LITERAL (right))
    return;

  struct valinfo leftv = getOperandValinfo (ic, left);
  struct valinfo rightv = getOperandValinfo (ic, right);
  struct valinfo resultv = *ic->resultvalinfo;
  
  optimizeNarrowOpCandidate (&leftv, left);
  optimizeNarrowOpCandidate (&rightv, right);
  optimizeNarrowResultCandidate (&resultv, result);

  valinfo_union (&resultv, leftv);
  valinfo_union (&resultv, rightv);

  if (resultv.anything || resultv.min < 0) // Todo: Relax this by also using signed _BitInt where doing so makes sense.
    return;

  unsigned int width = my_stdc_bit_width (resultv.max);
  width = ((width + 7) & (-8)); // Round up to multiple of 8.
  if (width >= bitsForType (operandType (result)))
    return;
  if (width > port->s.bitint_maxwidth)
    return;

#ifdef DEBUG_GCP_OPT
  std::cout << "Replacing operation at " << ic->key << ", by cheaper one on unsigned _BitInt(" << width << ")\n";
#endif

  sym_link *newtype = newBitIntLink (width);
  SPEC_USIGN (newtype) = true;

  reTypeOp (left, newtype);
  reTypeOp (right, newtype);
  reTypeOp (result, newtype);

  // Now also retype the other operand at some uses of the result.
  bitVect *uses = bitVectCopy (OP_USES (result));
  for (int key = bitVectFirstBit (uses); bitVectnBitsOn (uses); bitVectUnSetBit (uses, key), key = bitVectFirstBit (uses))
    {
      iCode *uic = (iCode *)hTabItemWithKey (iCodehTab, key);
      wassert (uic);
      if (uic->op == CAST) // todo: allow more: ifx, !.
        ;
      else if (uic->op == EQ_OP || uic->op == NE_OP ||
        uic->op == '<' || uic->op == LE_OP || uic->op == '>' || uic->op == GE_OP)
        {
        std::cout << "Replace other at " << ic->key << "\n";
          if (!isOperandEqual (result, uic->left))
            reTypeOp (uic->left, newtype);
          if (!isOperandEqual (result, uic->right))
            reTypeOp (uic->right, newtype);
        }
      else
        wassert (0);
    }
}

// Try to narrow operations
static void
optimizeValinfoNarrow (iCode *sic)
{
#ifdef DEBUG_GCP_OPT
  std::cout << "optimizeValifoNarrow at " << (currFunc ? currFunc->name : "[NOFUNC]") << "\n";
#endif
  for (iCode *ic = sic; ic; ic = ic->next)
    { 
      if (ic->op == '<' || ic->op == '>' || ic->op == LE_OP || ic->op == GE_OP || ic->op == EQ_OP || ic->op == NE_OP)
        optimizeBinaryOpWithoutResult (ic);
      else if (ic->op == '+' || ic->op == '-' || ic->op == '^' || ic->op == '|' || ic->op == BITWISEAND)
        optimizeBinaryOpWithResult (ic);
    }
}

// Do machine-independent optimizations based on valinfos.
void
optimizeValinfo (iCode *sic)
{
  optimizeValinfoConst (sic);
  optimizeValinfoNarrow (sic);
}

