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

#define DEBUG_GCP

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
#include "SDCCsymt.h"
#include "SDCCicode.h"
#include "SDCCBBlock.h"
#include "SDCCdflow.h"
#include "SDCClrange.h"
#include "SDCCy.h"
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

  if (IS_UNSIGNED (type) && bitsForType (type) < 64)
    {
      v.anything = false;
      v.min = 0;
      v.max = 0xffffffffffffffffull >> (64 - bitsForType (type));
    }
  else if (!IS_UNSIGNED (type) && bitsForType (type) < 63)
    {
      v.anything = false;
      v.max = 0x7fffffffffffffffull >> (64 - bitsForType (type));
      v.min = -v.max - 1;
    }
  return (v);
}

struct valinfo
getOperandValinfo (const iCode *ic, operand *op)
{
  wassert (ic);

  struct valinfo v;
  v.anything = true;
  v.nothing = false;
  v.min = v.max = 0;

  if (!op)
    return (v);

  sym_link *type = operandType (op);

  if (IS_OP_LITERAL (op) && !IS_FLOAT (type) && bitsForType (type) < 64) // Todo: More exact check than this bits thing?
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
      name[i] = os.str();
    }
  boost::write_graphviz(dump_file, cfg, boost::make_label_writer(name), boost::default_writer(), cfg_titlewriter(currFunc->rname, " generalized constant propagation"));
  delete[] name;
}

static void
valinfoPlus (struct valinfo *result, const struct valinfo left, const struct valinfo right)
{
  if (!left.anything && !right.anything &&
    llabs(left.min) > -1000 && llabs(right.min) > -1000 &&
    llabs(left.max) < +1000 && llabs(right.max) < +1000)
    {
      result->anything = false;
      result->nothing = left.nothing || right.nothing;
      auto min = left.min + right.min;
      auto max = left.max + right.max;
      if (min >= result->min && max <= result->max)
        {
          result->min = min;
          result->max = max;
        }
    }
}

static void
valinfoMinus (struct valinfo *result, const struct valinfo left, const struct valinfo right)
{
  if (!left.anything && !right.anything &&
    llabs(left.min) > -1000 && llabs(right.min) > -1000 &&
    llabs(left.max) < +1000 && llabs(right.max) < +1000)
    {
      result->anything = false;
      result->nothing = left.nothing || right.nothing;
      auto min = left.min - right.max;
      auto max = left.max - right.min;
      if (min >= result->min && max <= result->max)
        {
          result->min = min;
          result->max = max;
        }
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
      if (ic->op == IFX && bitVectnBitsOn (OP_DEFS (IC_COND (ic))) == 1) // Propagate some info on the condition into the branches.
        {
          iCode *cic = (iCode *)hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_DEFS (IC_COND (ic))));
          struct valinfo v = getOperandValinfo (ic, cic->left);
          if (cic->op == '>' && IS_ITEMP (cic->left) && !IS_FLOAT (operandType (cic->left)) &&
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
        }
      break;
    default:
      G[*out] = *ic->valinfos;

      if (resultsym)
        resultvalinfo = getTypeValinfo (operandType (IC_RESULT (ic)));

#ifdef DEBUG_GCP
     if (localchange)
       std::cout << "Recompute node " << i << " ic " << ic->key << "\n";
#endif

      if (!localchange) // Input didn't change. No need to recompute result.
        resultsym = 0;
      else if (end_it_quickly) // Just use the very rough approximation from the type info only to speed up analysis.
        ;
      else if (ic->op == '<' || ic->op == GE_OP)
        {
          resultvalinfo.nothing = leftvalinfo.nothing || rightvalinfo.nothing;
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
      else if ((ic->op == NE_OP || ic->op == EQ_OP) && !leftvalinfo.anything && !rightvalinfo.anything &&
        leftvalinfo.min == leftvalinfo.max && rightvalinfo.min == rightvalinfo.max)
        {
          bool one = (leftvalinfo.min == rightvalinfo.min) ^ (ic->op == NE_OP);
          resultvalinfo.anything = false;
          resultvalinfo.nothing = leftvalinfo.nothing || rightvalinfo.nothing;
          resultvalinfo.min = one;
          resultvalinfo.max = one;
        }
      else if (ic->op == '+')
        valinfoPlus (&resultvalinfo, leftvalinfo, rightvalinfo);
      else if (ic->op == '-')
        valinfoMinus (&resultvalinfo, leftvalinfo, rightvalinfo);
      else if (ic->op == '=' && !POINTER_SET (ic))
        resultvalinfo = rightvalinfo;

      if (resultsym)
        G[*out].map[resultsym->key] = resultvalinfo;
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
      recompute_node (G, i, ebbi, todo, true, false);
    }

  // Forward pass to get first approximation.
  for (unsigned int round = 0; !todo.first.empty (); round++)
    {
      // Take next node that needs updating.
      unsigned int i = todo.first.front ();
      todo.first.pop ();
      todo.second.erase (i);

#ifdef DEBUG_GCP
      std::cout << "Round " << round << " node " << i << " ic " << G[i].ic->key << "\n";
#endif
      recompute_node (G, i, ebbi, todo, false, round >= max_rounds);
    }

  // Refinement backward pass.
  // TODO

  if(options.dump_graphs)
    dump_cfg_genconstprop(G);
}

// Do optimizations based on valinfos.
void
optimizeValinfo (iCode *sic)
{
  for (iCode *ic = sic; ic; ic = ic->next)
    {
      if (SKIP_IC2 (ic))
        ;
      else
        {
          operand *left = IC_LEFT (ic);
          operand *right = IC_RIGHT (ic);
          operand *result = IC_RESULT (ic);
          valinfo vleft = getOperandValinfo (ic, left);
          valinfo vright = getOperandValinfo (ic, right);
          if (left && IS_ITEMP (left) && !vleft.anything && vleft.min == vleft.max)
            {
#ifdef DEBUG_GCP
              std::cout << "replace left (" << OP_SYMBOL(left)->name << "), key " << left->key << " at " << ic->key << "\n";std::cout << "anything " << vleft.anything << " min " << vleft.min << " max " << vleft.max << "\n";
#endif
              bitVectUnSetBit (OP_USES (left), ic->key);
              ic->left = operandFromValue (valCastLiteral (operandType (left), vleft.min, vleft.min), false);
            }
          if (right && IS_ITEMP (right) && !vright.anything && vright.min == vright.max)
            {
#ifdef DEBUG_GCP
              std::cout << "replace right at " << ic->key << "\n";std::cout << "anything " << vleft.anything << " min " << vleft.min << " max " << vleft.max << "\n";
#endif
              bitVectUnSetBit (OP_USES (right), ic->key);
              ic->right = operandFromValue (valCastLiteral (operandType (right), vright.min, vright.min), false);
            }
          if (ic->op == '+' || ic->op == '-')
            {
              valinfo vresult = getTypeValinfo (operandType (result));
              if (ic->op == '+')
                valinfoPlus (&vresult, vleft, vright);
              else
                valinfoMinus (&vresult, vleft, vright);
              if (!vresult.anything && vresult.min == vresult.max)
                {
#ifdef DEBUG_GCP
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
            }
          
        }
    }
}

