// Philipp Klaus Krause, philipp@informatik.uni-frankfurt.de, pkk@spth.de, 2012
//
// (c) 2012 Goethe-Universität Frankfurt
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
// Lifetime-optimal speculative partial redundancy elimination.

// #define DEBUG_LOSPRE // Uncomment to get debug messages while doing lospre.
// #define DEBUG_LOSPRE_ASS // Uncomment to get debug messages on considered assignmentd while doing lospre.

#include "SDCClospre.hpp"

#include <ctime>

// A quick-and-dirty function to get the CFG from sdcc (a simplified version of the function from SDCCralloc.hpp).
void
create_cfg_lospre (cfg_lospre_t &cfg, iCode *start_ic, ebbIndex *ebbi)
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
      if((ic->op == '>' || ic->op == '<' || ic->op == LE_OP || ic->op == GE_OP || ic->op == EQ_OP || ic->op == NE_OP || ic->op == '^' || ic->op == '|' || ic->op == BITWISEAND) && ifxForOp (IC_RESULT (ic), ic))
        boost::add_edge(key_to_index[ic->key], key_to_index[ic->next->key], 4.0f, cfg); // Try not to separate op from ifx.
      else if (ic->op != GOTO && ic->op != RETURN && ic->op != JUMPTABLE && ic->next)
        boost::add_edge(key_to_index[ic->key], key_to_index[ic->next->key], 3.0f, cfg);

      if (ic->op == GOTO)
        boost::add_edge(key_to_index[ic->key], key_to_index[eBBWithEntryLabel(ebbi, ic->label)->sch->key], 6.0f, cfg);
      else if (ic->op == RETURN)
        boost::add_edge(key_to_index[ic->key], key_to_index[eBBWithEntryLabel(ebbi, returnLabel)->sch->key], 6.0f, cfg);
      else if (ic->op == IFX)
        boost::add_edge(key_to_index[ic->key], key_to_index[eBBWithEntryLabel(ebbi, IC_TRUE(ic) ? IC_TRUE(ic) : IC_FALSE(ic))->sch->key], 6.0f, cfg);
      else if (ic->op == JUMPTABLE)
        for (symbol *lbl = (symbol *)(setFirstItem (IC_JTLABELS (ic))); lbl; lbl = (symbol *)(setNextItem (IC_JTLABELS (ic))))
          boost::add_edge(key_to_index[ic->key], key_to_index[eBBWithEntryLabel(ebbi, lbl)->sch->key], 6.0f, cfg);
    }
}

static bool
candidate_expression (const iCode *const ic, int lkey)
{
  wassert(ic);

  if (
    ic->op != '!' &&
    ic->op != '~' &&
    ic->op != UNARYMINUS &&
    ic->op != '+' &&
    ic->op != '-' &&
    ic->op != '*' &&
    ic->op != '/' &&
    ic->op != '%' &&
    ic->op != '>' &&
    ic->op != '<' &&
    ic->op != LE_OP &&
    ic->op != GE_OP &&
    ic->op != NE_OP &&
    ic->op != EQ_OP &&
    ic->op != AND_OP &&
    ic->op != OR_OP &&
    ic->op != '^' &&
    ic->op != '|' &&
    ic->op != BITWISEAND &&
    ic->op != RRC &&
    ic->op != RLC &&
    ic->op != GETABIT &&
    ic->op != GETHBIT &&
    ic->op != LEFT_OP &&
    ic->op != RIGHT_OP &&
    !(ic->op == '=' && !POINTER_SET(ic) && !(IS_ITEMP(IC_RIGHT(ic)) /*&& IC_RIGHT(ic)->key > lkey*/)) &&
    ic->op != GET_VALUE_AT_ADDRESS &&
    ic->op != CAST /*&&
    ic->op != ADDRESS_OF Apparently typically not worth the cost in code size*/)
    return (false);

  const operand *const left = IC_LEFT (ic);
  const operand *const right = IC_RIGHT (ic);
  const operand *const result = IC_RESULT (ic);

  // Todo: Allow literal right operand once backends can rematerialize literals!
  if(ic->op == '=' && IS_OP_LITERAL (right))
    return (false);

  // Todo: Allow more operands!
  if (ic->op != CAST && left && !(IS_SYMOP (left) || IS_OP_LITERAL (left)) ||
    right && !(IS_SYMOP (right) || IS_OP_LITERAL (right)) ||
    result && !(IS_SYMOP (result) || IS_OP_LITERAL (result)))
    return (false);

  return (true);
}

static bool
same_expression (const iCode *const lic, const iCode *const ric)
{
  wassert(lic);
  wassert(ric);

  if (lic->op != ric->op)
    return (false);

  const operand *lleft = IC_LEFT (lic);
  const operand *lright = IC_RIGHT (lic);
  const operand *lresult = IC_RESULT (lic);
  const operand *rleft = IC_LEFT (ric);
  const operand *rright = IC_RIGHT (ric);
  const operand *rresult = IC_RESULT (ric);

  if ((isOperandEqual (lleft, rleft) && isOperandEqual (lright, rright) ||
    IS_COMMUTATIVE (lic) && isOperandEqual (lleft, rright) && isOperandEqual (lright, rleft)) &&
    (lresult && rresult && compareTypeInexact (operandType (lresult), operandType (rresult)) > 0) &&
    IS_FLOAT (operandType (lresult)) == IS_FLOAT (operandType (rresult)))
    return (true);

  return (false);
}

static void
get_candidate_set(std::set<int> *c, const iCode *const sic, int lkey)
{
  // TODO: For loop invariant code motion allow expression that only occurs once, too - will be needed when optimizing for speed.
  for (const iCode *ic = sic; ic; ic = ic->next)
    {
      if (!candidate_expression (ic, lkey))
        continue;
      for (const iCode *pic = sic; pic != ic; pic = pic->next)
        if (candidate_expression (pic, lkey) && same_expression (ic, pic) && c->find (pic->key) == c->end () && pic->eBBlockNum != ic->eBBlockNum)
          {
            // Found expression that occurs at least twice.
            c->insert (pic->key);
            std::cout << "Found candidate at ic " << ic->key << " / " << pic->key << "\n";
            break;
          }
    }
}

static bool
invalidates_expression(const iCode *const eic, const iCode *const iic)
{
  const operand *const eleft = IC_LEFT (eic);
  const operand *const eright = IC_RIGHT (eic);
  const bool uses_global = (eic->op == GET_VALUE_AT_ADDRESS || isOperandGlobal (eleft) || isOperandGlobal (eright) || IS_SYMOP (eleft) && OP_SYMBOL_CONST (eleft)->addrtaken || IS_SYMOP (eright) && OP_SYMBOL_CONST (eright)->addrtaken);
  const bool uses_volatile = POINTER_GET (eic) && IS_VOLATILE (operandType (eleft)->next) || IS_OP_VOLATILE (eleft) || IS_OP_VOLATILE (eright);

  const operand *const left = IC_LEFT (iic);
  const operand *const right = IC_RIGHT (iic);
  const operand *const result = IC_RESULT (iic);

  if (iic->op == FUNCTION || iic->op == ENDFUNCTION || iic->op == RECEIVE)
    return(true);
  if (eic->op == ADDRESS_OF) // ADDRESS_OF does not really read its operand.
    return(false);
  if (eic->op == GET_VALUE_AT_ADDRESS && (isOperandGlobal (IC_RESULT (iic)) || IS_SYMOP (IC_RESULT (iic)) && OP_SYMBOL_CONST (IC_RESULT (iic))->addrtaken))
    return(true);
  if (IC_RESULT (iic) && !IS_OP_LITERAL (result) && !POINTER_SET(iic) &&
    (eleft && isOperandEqual (eleft, result) || eright && isOperandEqual (eright, result)))
    return(true);
  if ((uses_global || uses_volatile) && (iic->op == CALL || iic->op == PCALL))
    return(true);
  if (uses_volatile && (POINTER_GET (iic) && IS_VOLATILE (operandType (left)->next)) || IS_OP_VOLATILE (left) || IS_OP_VOLATILE (right))
    return(true);
  if (uses_global && POINTER_SET (iic)) // TODO: More accuracy here!
    return(true);

  return(false);
}

#if 0
static bool
setup_cfg_for_expression (cfg_lospre_t *const cfg, const iCode *const eic)
{
  typedef boost::graph_traits<cfg_lospre_t>::vertex_descriptor vertex_t;
  bool safety_required = false;

  // In redundancy elimination, safety means not doing a computation on any path were it was not done before.
  // This is important, if the compuation can have side-effects, which depends on the target architecure.
  // E.g. On some systems division requires safety, since division by zero might result in an interrupt.
  // When there are memory-mapped devices or there is memory management, reading from a pointer requires
  // safety, since reading from an unknown location could result in making the device do something or in a SIGSEGV. 
  // On the other hand, addition is something that typically does not require safety, since adding two undefined
  // operands gives just another undefined (the C standard allows trap representations, which, could result
  // in addition requiring safety though; AFAIK none of the targets currently supported by SDCC have trap representations).
  // Philipp, 2012-07-06.
  //
  // For now we just always require safety for "dangerous" operations.
  //
  // TODO: Replace the current one  by a more exact mechanism, that takes into account information from
  // (not yet implemented) generalized constant propagation, pointer analysis, etc.

  // Function calls can have any side effects.
  if (eic->op == CALL || eic->op == PCALL)
    safety_required = true;

  // volatile requires safety.
  if (POINTER_GET (eic) && IS_VOLATILE (operandType (IC_LEFT (eic))->next) || IS_OP_VOLATILE (IC_LEFT (eic)) || IS_OP_VOLATILE (IC_RIGHT (eic)))
    safety_required = true;

  // Reading from an invalid address might be dangerous, since there could be memory-mapped I/O.
  if (eic->op == GET_VALUE_AT_ADDRESS && !optimize.allow_unsafe_read)
    safety_required = true;

  // The division routines for z80-like ports and the hc08/s08's and stm8's hardware division just give an undefined result
  // for division by zero, but there are no harmful side effects. I don't know about the other ports.
  if ((eic->op == '/' || eic->op == '%') && !TARGET_Z80_LIKE && !TARGET_HC08_LIKE && !TARGET_IS_STM8)
    safety_required = true;

  // TODO: Relax this! There are cases where allowing unsafe optimizations will improve speed.
  // This probably needs implementation of profile-guided optimization though.
  if (optimize.codeSpeed)
    safety_required = true;

#ifdef DEBUG_LOSPRE
  std::cout << "Invalidation set I: ";
#endif
  for (vertex_t i = 0; i < boost::num_vertices (*cfg); i++)
    {
       const iCode *const ic = (*cfg)[i].ic;

       (*cfg)[i].uses = same_expression (eic, ic);
       (*cfg)[i].invalidates = invalidates_expression (eic, ic);

       (*cfg)[i].forward = std::pair<int, int>(-1, -1);

#ifdef DEBUG_LOSPRE
       if ((*cfg)[i].invalidates)
         std::cout << i << ", ";
#endif
    }
#ifdef DEBUG_LOSPRE
  std::cout << "\n";
#endif

  return (safety_required);
}
#endif

// Dump cfg, with numbered nodes.
void dump_cfg_lospre (const cfg_lospre_t &cfg)
{
  if (!currFunc)
    return;

  std::ofstream dump_file((std::string(dstFileName) + ".dumplosprecfg" + currFunc->rname + ".dot").c_str());

  std::string *name = new std::string[num_vertices(cfg)];
  for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {
      const char *iLine = printILine (cfg[i].ic);
      std::ostringstream os;
      os << i << ", " << cfg[i].ic->key << " : " << iLine;
      dbuf_free (iLine);
      name[i] = os.str();
    }
  boost::write_graphviz(dump_file, cfg, boost::make_label_writer(name), boost::default_writer(), cfg_titlewriter(currFunc->rname, "lospre"));
  delete[] name;
}

// Dump tree decomposition.
static void dump_dec_lospre(const tree_dec_t &tree_dec)
{
  wassert (currFunc);

  std::ofstream dump_file((std::string(dstFileName) + ".dumplospredec" + currFunc->rname + ".dot").c_str());

  unsigned int w = 0;

  std::string *name = new std::string[num_vertices(tree_dec)];
  for (unsigned int i = 0; i < boost::num_vertices(tree_dec); i++)
    {
      if (tree_dec[i].bag.size() > w)
        w = tree_dec[i].bag.size();
      std::ostringstream os;
      typename decltype(tree_dec[0].bag)::const_iterator v1;
       os << i << " | ";
      for (v1 = tree_dec[i].bag.begin(); v1 != tree_dec[i].bag.end(); ++v1)
        os << *v1 << " ";
      name[i] = os.str();
    }
  boost::write_graphviz(dump_file, tree_dec, boost::make_label_writer(name));
  delete[] name;
}

struct bbcfg_lospre_node
{
  bool uses;
  bool invalidates;

  iCode *firstic;
};

struct bbcfg_mcpre_node
{
  bool antloc, avloc, transp, kill;
  bool n_aval, x_aval, x_pant, n_pant;
  bool x_live, n_live;
  bool d_isolated, d_insdel;
  bool u_isolated, u_del;

  iCode *firstic;
  int red_map;
};

struct bbcfg_mcpre_edge
{
  bool ins_redund, ins_useless, non_ess, ess;
  bool u_ins;

  bool c_lambda;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, bbcfg_lospre_node, float> bbcfg_lospre_t; // The edge property is the cost of subdividing the edge and inserting an instruction (for now we always use 1, optimizing for code size, but relative execution frequency could be used when optimizing for speed or total energy consumption; aggregates thereof can be a good idea as well).
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, bbcfg_mcpre_node, bbcfg_mcpre_edge> bbcfg_mcpre_t;

struct bbcfg_red_mcpre_node
{
  bool top, bot;

  unsigned int top_part, bot_part;

  int red_map_inv;
};

struct bbcfg_red_mcpre_edge
{
  std::pair<int, int> gamma;

  boost::graph_traits<bbcfg_mcpre_t>::edge_iterator red_map_inv;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, bbcfg_red_mcpre_node, bbcfg_red_mcpre_edge> bbcfg_red_mcpre_t;

struct bbcfg_red2_mcpre_node
{
};

struct bbcfg_red2_mcpre_edge
{
  boost::graph_traits<bbcfg_mcpre_t>::edge_iterator gamma_inv;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, bbcfg_red2_mcpre_node, bbcfg_red2_mcpre_edge> bbcfg_red2_mcpre_t;

struct bbcfg_flow_mcpre_node
{
  bool reaches_end;
};

struct bbcfg_flow_mcpre_edge
{
  boost::graph_traits<bbcfg_mcpre_t>::edge_iterator gamma_inv;

  unsigned int capacity;
  unsigned int residual_capacity;

  bool c_lambda;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, bbcfg_flow_mcpre_node, bbcfg_flow_mcpre_edge> bbcfg_flow_mcpre_t;

void
create_bbcfg_lospre (bbcfg_lospre_t &cfg, iCode *start_ic, ebbIndex *ebbi)
{
  iCode *ic;

  // Make a node for each basic block.
  std::map<int, unsigned int> key_to_index;
  {
    int i = -3;
    bool adding = false;

    for (ic = start_ic; ic; ic = ic->next)
      {
        if (ic->op == LABEL)
          adding = false;
        if (!adding)
        {
          boost::add_vertex(cfg);
          boost::add_vertex(cfg);
          boost::add_vertex(cfg);
          i += 3;
          cfg[i].firstic = ic;
          cfg[i + 1].firstic = 0;
          cfg[i + 2].firstic = 0;
          boost::add_edge(i, i + 1, 3.0f, cfg);
          boost::add_edge(i + 1, i + 2, 3.0f, cfg);
          adding = true;
        }    
        if (ic->op == IFX || ic->op == GOTO || ic->op == JUMPTABLE)
          adding = false;

        key_to_index[ic->key] = i;
      }
  }

  // Get control flow graph from sdcc.
  for (ic = start_ic; ic; ic = ic->next)
    {
      if((ic->op == '>' || ic->op == '<' || ic->op == LE_OP || ic->op == GE_OP || ic->op == EQ_OP || ic->op == NE_OP || ic->op == '^' || ic->op == '|' || ic->op == BITWISEAND) && ifxForOp (IC_RESULT (ic), ic) && key_to_index[ic->key] != key_to_index[ic->next->key])
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[ic->next->key], 4.0f, cfg); // Try not to separate op from ifx.
      else if (ic->op != GOTO && ic->op != RETURN && ic->op != JUMPTABLE && ic->next && key_to_index[ic->key] != key_to_index[ic->next->key])
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[ic->next->key], 3.0f, cfg);

      if (ic->op == GOTO)
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[eBBWithEntryLabel(ebbi, ic->label)->sch->key], 6.0f, cfg);
      else if (ic->op == RETURN)
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[eBBWithEntryLabel(ebbi, returnLabel)->sch->key], 6.0f, cfg);
      else if (ic->op == IFX)
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[eBBWithEntryLabel(ebbi, IC_TRUE(ic) ? IC_TRUE(ic) : IC_FALSE(ic))->sch->key], 6.0f, cfg);
      else if (ic->op == JUMPTABLE)
        for (symbol *lbl = (symbol *)(setFirstItem (IC_JTLABELS (ic))); lbl; lbl = (symbol *)(setNextItem (IC_JTLABELS (ic))))
          boost::add_edge(key_to_index[ic->key] + 2, key_to_index[eBBWithEntryLabel(ebbi, lbl)->sch->key], 6.0f, cfg);
    }
}

static void
setup_bbcfg_lospre_for_expression (bbcfg_lospre_t *const cfg, const iCode *const eic)
{
  for (unsigned int i = 0; i < boost::num_vertices(*cfg); i += 3)
    {
      (*cfg)[i].uses = false; (*cfg)[i + 1].uses = false; (*cfg)[i + 2].uses = false;
      (*cfg)[i].invalidates = false; (*cfg)[i + 1].invalidates = false; (*cfg)[i + 2].invalidates = false;

      for (const iCode *ic = (*cfg)[i].firstic; ic; ic = ic->next)
        {
          if (same_expression (eic, ic))
            (*cfg)[i + 1 + (*cfg)[i + 1].invalidates].uses = true;

          if (invalidates_expression (eic, ic))
            {
              (*cfg)[i + 1].invalidates = true;
              (*cfg)[i + 2].uses = false;
            }

          if ((ic->op == IFX || ic->op == GOTO || ic->op == JUMPTABLE) || ic->next && ic->next->op == LABEL)
            break;
        }
    }
}

void
create_bbcfg_mcpre (bbcfg_mcpre_t &cfg, iCode *start_ic, ebbIndex *ebbi)
{
  iCode *ic;

  // Make a node for each basic block.
  std::map<int, unsigned int> key_to_index;
  {
    int i = -3;
    bool adding = false;

    for (ic = start_ic; ic; ic = ic->next)
      {
        if (ic->op == LABEL)
          adding = false;
        if (!adding)
        {
          boost::add_vertex(cfg); // This extra node is not used in the MC-PRE paper, but is necessary to be able to do optimization for code size.
          boost::add_vertex(cfg);
          boost::add_vertex(cfg); // This extra node is not used in the MC-PRE paper, but is necessary to be able to do optimization for code size.
          i += 3;
          boost::add_edge(i + 0, i + 1, cfg);
          boost::add_edge(i + 1, i + 2, cfg);
          cfg[i + 0].firstic = 0;
          cfg[i + 1].firstic = ic;
          cfg[i + 2].firstic = 0;
          adding = true;
        }    
        if (ic->op == IFX || ic->op == GOTO || ic->op == JUMPTABLE)
          adding = false;

        key_to_index[ic->key] = i;
      }
  }

  // Get control flow graph from sdcc.
  for (ic = start_ic; ic; ic = ic->next)
    {
      if((ic->op == '>' || ic->op == '<' || ic->op == LE_OP || ic->op == GE_OP || ic->op == EQ_OP || ic->op == NE_OP || ic->op == '^' || ic->op == '|' || ic->op == BITWISEAND) && ifxForOp (IC_RESULT (ic), ic) && key_to_index[ic->key] != key_to_index[ic->next->key])
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[ic->next->key], cfg); // Try not to separate op from ifx.
      else if (ic->op != GOTO && ic->op != RETURN && ic->op != JUMPTABLE && ic->next && key_to_index[ic->key] != key_to_index[ic->next->key])
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[ic->next->key], cfg);

      if (ic->op == GOTO)
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[eBBWithEntryLabel(ebbi, ic->label)->sch->key], cfg);
      else if (ic->op == RETURN)
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[eBBWithEntryLabel(ebbi, returnLabel)->sch->key], cfg);
      else if (ic->op == IFX)
        boost::add_edge(key_to_index[ic->key] + 2, key_to_index[eBBWithEntryLabel(ebbi, IC_TRUE(ic) ? IC_TRUE(ic) : IC_FALSE(ic))->sch->key], cfg);
      else if (ic->op == JUMPTABLE)
        for (symbol *lbl = (symbol *)(setFirstItem (IC_JTLABELS (ic))); lbl; lbl = (symbol *)(setNextItem (IC_JTLABELS (ic))))
          boost::add_edge(key_to_index[ic->key] + 2, key_to_index[eBBWithEntryLabel(ebbi, lbl)->sch->key], cfg);
    }
}

static void
setup_bbcfg_mcpre_for_expression (bbcfg_mcpre_t *const cfg, const iCode *const eic)
{
  for (unsigned int i = 0; i < boost::num_vertices(*cfg); i++)
    {
      (*cfg)[i].kill = false;
      (*cfg)[i].avloc = false;
      (*cfg)[i].antloc = false;

      for (const iCode *ic = (*cfg)[i].firstic; ic; ic = ic->next)
        {
          if (!(*cfg)[i].kill && same_expression (eic, ic))
            (*cfg)[i].antloc = true;

          if (invalidates_expression (eic, ic))
            {
              (*cfg)[i].kill = true;
              (*cfg)[i].avloc = false;
            }

          if (same_expression (eic, ic))
            (*cfg)[i].avloc = true;

          if ((ic->op == IFX || ic->op == GOTO || ic->op == JUMPTABLE) || ic->next && ic->next->op == LABEL)
            break;
        }

      (*cfg)[i].transp = !(*cfg)[i].kill;
    }
}

void dump_bbcfg_lospre (const bbcfg_lospre_t &cfg)
{
  if (!currFunc)
    return;

  std::ofstream dump_file((std::string(dstFileName) + ".dumplosprebbcfg" + currFunc->rname + ".dot").c_str());

  std::string *name = new std::string[num_vertices(cfg)];
  for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {
      std::ostringstream os;
      os << i;
      if (cfg[i].firstic)
        os << " First ic key " << cfg[i].firstic->key;
      name[i] = os.str();
    }
  boost::write_graphviz(dump_file, cfg, boost::make_label_writer(name), boost::default_writer(), cfg_titlewriter(currFunc->rname, "bb-lospre"));
  delete[] name;
}

// Dump CFG used for lospre with expression-specific node labels
void dump_bbcfg_lospre_ (const bbcfg_lospre_t &cfg)
{
  if (!currFunc)
    return;

  static int n;

  std::string *name = new std::string[num_vertices(cfg)];
  for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {
      std::ostringstream os;
      os << i << " uses: " << cfg[i].uses << " invalidates: " << cfg[i].invalidates;
      name[i] = os.str();
    }

  std::ostringstream filename;
  filename << std::string(dstFileName) << ".dumplosprebbcfg" << "-" << n++ << currFunc->rname << ".dot";
  std::ofstream dump_file(filename.str().c_str());

  boost::write_graphviz(dump_file, cfg, boost::make_label_writer(name), boost::default_writer(), cfg_titlewriter(currFunc->rname, "bb-lospre"));
  delete[] name;
}

void dump_bbcfg_mcpre(const bbcfg_mcpre_t &cfg)
{
  if (!currFunc)
    return;

  std::ofstream dump_file((std::string(dstFileName) + ".dumpmcprebbcfg" + currFunc->rname + ".dot").c_str());

  std::string *name = new std::string[num_vertices(cfg)];
  for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {
      std::ostringstream os;
      os << i;
      if (cfg[i].firstic)
        os << " First ic key " << cfg[i].firstic->key;
      name[i] = os.str();
    }
  boost::write_graphviz(dump_file, cfg, boost::make_label_writer(name), boost::default_writer(), cfg_titlewriter(currFunc->rname, "bb-mcpre"));
  delete[] name;
}

// Dump CFG used for MC-PRE with expression-specific node labels
void dump_bbcfg_mcpre_(const bbcfg_mcpre_t &cfg)
{
  if (!currFunc)
    return;

  static int n;

  std::string *name = new std::string[num_vertices(cfg)];
  for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {
      std::ostringstream os;
      os << i << " AVLOC: " << cfg[i].avloc << " ANTLOC: " << cfg[i].antloc << " KILL: " << cfg[i].kill;
      name[i] = os.str();
    }

  std::ostringstream filename;
  filename << std::string(dstFileName) << ".dumpmcprebbcfg" << "-" << n++ << currFunc->rname << ".dot";
  std::ofstream dump_file(filename.str().c_str());

  boost::write_graphviz(dump_file, cfg, boost::make_label_writer(name), boost::default_writer(), cfg_titlewriter(currFunc->rname, "bb-mcpre"));
  delete[] name;
}

// Dump expression-speific reduced CFG used for MC-PRE
void dump_bbcfg_red_mcpre_(const bbcfg_red_mcpre_t &cfg)
{
  if (!currFunc)
    return;

  static int n;

  std::ostringstream filename;
  filename << std::string(dstFileName) << ".dumpmcprebbcfg_red" << "-" << n++ << currFunc->rname << ".dot";
  std::ofstream dump_file(filename.str().c_str());

  boost::write_graphviz(dump_file, cfg, boost::default_writer(), boost::default_writer(), cfg_titlewriter(currFunc->rname, "bb-mcpre (reduced)"));
}

// Dump expression-speific reduced CFG used for MC-PRE
void dump_bbcfg_red2_mcpre_(const bbcfg_red2_mcpre_t &cfg)
{
  if (!currFunc)
    return;

  static int n;

  std::ostringstream filename;
  filename << std::string(dstFileName) << ".dumpmcprebbcfg_red2" << "-" << n++ << currFunc->rname << ".dot";
  std::ofstream dump_file(filename.str().c_str());

  boost::write_graphviz(dump_file, cfg, boost::default_writer(), boost::default_writer(), cfg_titlewriter(currFunc->rname, "bb-mcpre (reduced s-t)"));
}

template <class G_t, class T_t>
void bb_lospre(const G_t &G, T_t& T, const iCode *ic)
{
  if (options.dump_graphs)
    dump_bbcfg_lospre_(G);

  const boost::graph_traits<tree_dec_t>::vertex_descriptor t_root = find_root(T);

  tree_dec_lospre_nodes(T, t_root, G);

  const assignment_lospre &winner = *(T[t_root].assignments.begin());

#ifdef DEBUG_LOSPRE
  std::cout << "Winner (lospre): ";
  print_assignment(winner, G);
#endif

  typedef typename boost::graph_traits<bbcfg_lospre_t>::edge_iterator edge_iter_t;
  edge_iter_t e, e_end;
  for(boost::tie(e, e_end) = boost::edges(G); e != e_end; ++e)
    if(!((winner.global[boost::source(*e, G)] & true) && !G[boost::source(*e, G)].invalidates) && (winner.global[boost::target(*e, G)] & true))
      std::cout << "lospre would insert calculation at (" << boost::source(*e, G) << ", " << boost::target(*e, G) << ")\n";

  T[t_root].assignments.clear();
}

#include <boost/graph/push_relabel_max_flow.hpp>

int bb_mcpre(bbcfg_mcpre_t &cfg, const iCode *ic, const bool elim_only)
{
  typedef typename boost::graph_traits<bbcfg_mcpre_t>::in_edge_iterator in_iter_t;
  typedef typename boost::graph_traits<bbcfg_mcpre_t>::out_edge_iterator out_iter_t;
  typedef typename boost::graph_traits<bbcfg_mcpre_t>::edge_iterator edge_iter_t;

  if(options.dump_graphs && !elim_only)
    dump_bbcfg_mcpre_(cfg);

  // 1. Data-flow analysis (step 3.1 of MC-PRE_comp)
  // 3.1a forward
  for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {
      cfg[i].n_aval = true;
      cfg[i].x_aval = true;
    }
  for(bool change = true; change;)
    {
      change = false;
      for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
        {
          const bool old_n_aval = cfg[i].n_aval;
          const bool old_x_aval = cfg[i].x_aval;

          if(!in_degree(i, cfg))
            cfg[i].n_aval = false;
          else
            {
              in_iter_t in, in_end;
              cfg[i].n_aval = true;
              for (boost::tie(in, in_end) = boost::in_edges(i, cfg);  in != in_end; ++in)
                cfg[i].n_aval = cfg[i].n_aval && cfg[boost::source(*in, cfg)].x_aval;
            }
          cfg[i].x_aval = cfg[i].avloc || cfg[i].n_aval && cfg[i].transp;

          if (cfg[i].n_aval != old_n_aval || cfg[i].x_aval != old_x_aval)
            change = true;
        }
    }
  // 3.1b backward
  for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {
      cfg[i].x_pant = false;
      cfg[i].n_pant = false;
    }
  for(bool change = true; change;)
    {
      change = false;
      for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
        {
          const bool old_x_pant = cfg[i].x_pant;
          const bool old_n_pant = cfg[i].n_pant;

          out_iter_t out, out_end;
          cfg[i].x_pant = false;
          for (boost::tie(out, out_end) = boost::out_edges(i, cfg);  out != out_end; ++out)
            cfg[i].x_pant = cfg[i].x_pant || cfg[boost::target(*out, cfg)].n_pant;
          cfg[i].n_pant = cfg[i].antloc || cfg[i].x_pant && cfg[i].transp;

          if (cfg[i].x_pant != old_x_pant || cfg[i].n_pant != old_n_pant)
            change = true;
        }
    }

  // 2. Obtain G_rd (step 3.2 of MC-PRE_comp)
  bbcfg_red_mcpre_t G_rd;
  // 3.2a
  {
    edge_iter_t e, e_end;
    for(boost::tie(e, e_end) = boost::edges(cfg); e != e_end; ++e)
      {
        cfg[*e].ins_redund = cfg[boost::source(*e, cfg)].x_aval;
        cfg[*e].ins_useless = !cfg[boost::target(*e, cfg)].n_pant;
        cfg[*e].non_ess = cfg[boost::source(*e, cfg)].x_aval || !cfg[boost::target(*e, cfg)].n_pant;
        cfg[*e].ess = !cfg[*e].non_ess;
      }
  }
  // 3.2b
  {
    for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
      cfg[i].red_map = -1;

    int i = -1;
    edge_iter_t e, e_end;
    for(boost::tie(e, e_end) = boost::edges(cfg); e != e_end; ++e)
      {
        if (!cfg[*e].ess)
          continue;

        if(cfg[boost::source(*e, cfg)].red_map < 0)
          {
            boost::add_vertex(G_rd);
            i++;
            cfg[boost::source(*e, cfg)].red_map = i;
            G_rd[i].red_map_inv = boost::source(*e, cfg);
          }
        if(cfg[boost::target(*e, cfg)].red_map < 0)
          {
            boost::add_vertex(G_rd);
            i++;
            cfg[boost::target(*e, cfg)].red_map = i;
            G_rd[i].red_map_inv = boost::target(*e, cfg);
          }
        G_rd[boost::add_edge(cfg[boost::source(*e, cfg)].red_map, cfg[boost::target(*e, cfg)].red_map, G_rd).first].red_map_inv = e;
      }
  }

  if(options.dump_graphs && !elim_only)
    dump_bbcfg_red_mcpre_(G_rd);

  // 3. Obtain G_mm (step 3.3 of MC-PRE_comp)
  bbcfg_red2_mcpre_t G;
  unsigned int s_mm, t_mm;
  {
    typedef typename boost::graph_traits<bbcfg_red_mcpre_t>::edge_iterator edge_iter_t;

    unsigned int n = boost::num_vertices(G_rd);
    edge_iter_t e, e_end;
  // 3.3a
    for (unsigned int i = 0; i < boost::num_vertices(G_rd); i++)
      {
        G_rd[i].top = cfg[G_rd[i].red_map_inv].antloc && in_degree(i, G_rd);
        G_rd[i].bot = cfg[G_rd[i].red_map_inv].kill && out_degree(i, G_rd);
        G_rd[i].top_part = /*G_rd[i].top && G_rd[i].bot ? n++ :*/ i; /* We always reuse  i (instead of porentially introducing a new node i+ and deleting  i */
        G_rd[i].bot_part = G_rd[i].top && G_rd[i].bot ? n++ : i;
      }

    for(boost::tie(e, e_end) = boost::edges(G_rd); e != e_end; ++e)
      G_rd[*e].gamma = std::pair<int, int>(G_rd[boost::source(*e, G_rd)].bot_part, G_rd[boost::target(*e, G_rd)].top_part);
  // 3.3b
    for (unsigned int i = 0; i < n; i++)
      boost::add_vertex(G);
    for(boost::tie(e, e_end) = boost::edges(G_rd); e != e_end; ++e)
      G[boost::add_edge(G_rd[*e].gamma.first, G_rd[*e].gamma.second, G).first].gamma_inv = G_rd[*e].red_map_inv;
  // 3.3c
    s_mm = 0;
    t_mm = 0;
    for(unsigned int i = 0; i < boost::num_vertices(G); i++)
      {
        if (!in_degree(i, G))
          s_mm++;
        if (!out_degree(i, G))
          t_mm++;
      }
  }

  // 4. Obtain G_st (step 3.4 of MC-PRE_comp)
  int s, t;
  {
    unsigned int n = boost::num_vertices(G);
    s = s_mm ? boost::add_vertex(G) : -1;
    t = t_mm ? boost::add_vertex(G) : -1;

    for(unsigned int i = 0; i < n; i++)
      {
        if (!in_degree(i, G))
          add_edge(s, i, G);
        if (!out_degree(i, G))
          add_edge(i, t, G);
      }
  }

  if(s < 0 || t < 0)
    {
      //std::cout << "Invalid reduced CFG. Abort MC-PRE.\n";
      return(-1);
    }
  else if (elim_only)
    return(0);

  if(options.dump_graphs)
    dump_bbcfg_red2_mcpre_(G);

  // 5. Find unique minimum cut
  bbcfg_flow_mcpre_t G_flow;
  // 5.5a
  {
    typedef typename boost::graph_traits<bbcfg_flow_mcpre_t>::edge_descriptor edge_t;
    std::map<edge_t, edge_t> rev;

    for (unsigned int i = 0; i < boost::num_vertices(G); i++)
      boost::add_vertex(G_flow);

    typedef typename boost::graph_traits<bbcfg_red2_mcpre_t>::edge_iterator edge_iter_t;
    edge_iter_t e, e_end;
    for(boost::tie(e, e_end) = boost::edges(G); e != e_end; ++e)
      {
        edge_t edge = boost::add_edge(boost::source(*e, G), boost::target(*e, G), G_flow).first;
        edge_t rev_edge = boost::add_edge(boost::target(*e, G), boost::source(*e, G), G_flow).first;
        if (boost::source(*e, G) == unsigned(s) || boost::target(*e, G) == unsigned(t))
           G_flow[edge].capacity = 10000;
        else
          {
            G_flow[edge].gamma_inv = G[*e].gamma_inv;
            G_flow[edge].capacity = 1;
          }
        G_flow[rev_edge].capacity = 0;
        rev[edge] = rev_edge;
        rev[rev_edge] = edge;
      }

    boost::push_relabel_max_flow(G_flow, s, t, get(&bbcfg_flow_mcpre_edge::capacity, G_flow), get(&bbcfg_flow_mcpre_edge::residual_capacity, G_flow), boost::associative_property_map< std::map<edge_t, edge_t> >(rev), get(boost::vertex_index, G_flow));
  }
  // 5.5b,c
  {
    typedef typename boost::graph_traits<bbcfg_flow_mcpre_t>::edge_iterator edge_iter_t;
    edge_iter_t e, e_end;

    for (unsigned int i = 0; i < boost::num_vertices(G_flow); i++)
      G_flow[i].reaches_end = false;
    G_flow[t].reaches_end = true;
    for(bool change = true; change;)
      {
        change = false;
        for(boost::tie(e, e_end) = boost::edges(G_flow); e != e_end; ++e)
          if(!G_flow[boost::source(*e, G_flow)].reaches_end && G_flow[boost::target(*e, G_flow)].reaches_end && G_flow[*e].residual_capacity)
            {
              G_flow[boost::source(*e, G_flow)].reaches_end = true;
              change = true;
            }
      }
  }
  // 5.5d,e
  {
    typedef typename boost::graph_traits<bbcfg_flow_mcpre_t>::edge_iterator edge_iter_t;
    edge_iter_t e, e_end;

    for(boost::tie(e, e_end) = boost::edges(G_flow); e != e_end; ++e)
      {
        G_flow[*e].c_lambda = !G_flow[boost::source(*e, G_flow)].reaches_end && G_flow[boost::target(*e, G_flow)].reaches_end;
        //if(G_flow[*e].c_lambda)
        //  std::cout << "C_Lambda edge in red CFG: (" << boost::source(*e, cfg) << ", " << boost::target(*e, cfg) << ")\n"; std::cout.flush();
      }
  }
  // 5.5f
  edge_iter_t e, e_end;
  for(boost::tie(e, e_end) = boost::edges(cfg); e != e_end; ++e)
    cfg[*e].c_lambda = false;
  {
    typedef typename boost::graph_traits<bbcfg_flow_mcpre_t>::edge_iterator edge_iter_t;
    edge_iter_t e, e_end;

    for(boost::tie(e, e_end) = boost::edges(G_flow); e != e_end; ++e)
      if (G_flow[*e].c_lambda && G_flow[*e].capacity)
        {
          wassert (boost::source(*e, G_flow) != unsigned(s) && boost::target(*e, G_flow) != unsigned(t));
          cfg[*(G_flow[*e].gamma_inv)].c_lambda = true;
          //std::cout << "Transfer C_Lambda via gamma_inv from (" << boost::source(*e, G_flow) << ", " << boost::target(*e, G_flow) << ") to (" << boost::source(*(G_flow[*e].gamma_inv), cfg) << ", " << boost::target(*(G_flow[*e].gamma_inv), cfg) << ")\n";
        }
  }

  // 6. Backward data-flow analysis
   for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {
      cfg[i].x_live = false;//std::cout << "1 x_live at " << i << " : " << cfg[i].x_live << "\n";
      cfg[i].n_live = false;
    }
  for(bool change = true; change;)
    {
      change = false;
      for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
        {
          const bool old_x_live = cfg[i].x_live;
          const bool old_n_live = cfg[i].n_live;

          out_iter_t out, out_end;
          cfg[i].x_live = false;
          for (boost::tie(out, out_end) = boost::out_edges(i, cfg);  out != out_end; ++out)
            cfg[i].x_live = cfg[i].x_live || cfg[boost::target(*out, cfg)].n_live && !cfg[*e].c_lambda;
          cfg[i].n_live = cfg[i].antloc || cfg[i].x_live && cfg[i].transp;
//std::cout << "2 x_live at " << i << " : " << cfg[i].x_live << " (was) " << old_x_live << "\n";
          if (cfg[i].x_live != old_x_live || cfg[i].n_live != old_n_live)
            change = true;
        }
    }

  for(boost::tie(e, e_end) = boost::edges(cfg); e != e_end; ++e)
    if(cfg[*e].c_lambda)
      std::cout << "C_Lambda edge in CFG: (" << boost::source(*e, cfg) << ", " << boost::target(*e, cfg) << ")\n";

  // 7.
  for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {//std::cout << "3 x_live at " << i << " : " << cfg[i].x_live << "\n";
      cfg[i].d_isolated = !cfg[i].x_live;
      cfg[i].d_insdel = (cfg[i].avloc && cfg[i].kill) && !cfg[i].d_isolated;
    }

  // 8. TODO
  for (unsigned int i = 0; i < boost::num_vertices(cfg); i++)
    {
      in_iter_t in, in_end;
      cfg[i].u_isolated = (cfg[i].kill || !cfg[i].x_live);
      for(boost::tie(in, in_end) = boost::in_edges(i, cfg);  in != in_end; ++in)
        if(!cfg[*in].c_lambda) cfg[i].u_isolated = false;
      cfg[i].u_del = cfg[i].antloc && !cfg[i].u_isolated;
    }
  for(boost::tie(e, e_end) = boost::edges(cfg); e != e_end; ++e)
    cfg[*e].u_ins = cfg[*e].c_lambda && !cfg[boost::target(*e, cfg)].u_isolated;

  // Output result.
  for(boost::tie(e, e_end) = boost::edges(cfg); e != e_end; ++e)
{
    //if(cfg[*e].c_lambda)
    //   std::cout << "Edge: (" << boost::source(*e, cfg) << ", " << boost::target(*e, cfg) << ") target u_isolated: " << cfg[boost::target(*e, cfg)].u_isolated << ", x_live: " << cfg[boost::target(*e, cfg)].x_live << "\n";
    if(cfg[*e].u_ins)
      std::cout << "mc-pre would insert calculation at (" << boost::source(*e, cfg) << ", " << boost::target(*e, cfg) << ")\n";
}

  return(0);
}

// Eliminate trivial cases
std::set<int> bb_elim_all(const std::set<int>& candidate_set, iCode *sic, ebbIndex *ebbi)
{
  bbcfg_mcpre_t control_flow_graph;
  create_bbcfg_mcpre (control_flow_graph, sic, ebbi);

  std::set<int> result, elim_set;

  std::set<int>::iterator ci, ci_end;
  for (ci = candidate_set.begin(), ci_end = candidate_set.end(); ci != ci_end; ++ci)
    {
      const iCode *ic;

      for (ic = sic; ic && ic->key != *ci; ic = ic->next);
      if (!ic || !candidate_expression (ic, operandKey))
        continue;

      setup_bbcfg_mcpre_for_expression (&control_flow_graph, ic);

      if(bb_mcpre (control_flow_graph, ic, true) < 0)
        elim_set.insert(*ci);
    }
  std::set_difference(candidate_set.begin(), candidate_set.end(), elim_set.begin(), elim_set.end(), std::inserter(result, result.begin()));

  return(result);
}

void bb_lospre_all (const std::set<int>& candidate_set, iCode *sic, ebbIndex *ebbi)
{
  bbcfg_lospre_t control_flow_graph;
  create_bbcfg_lospre (control_flow_graph, sic, ebbi);

  if(options.dump_graphs)
    dump_bbcfg_lospre(control_flow_graph);

  std::clock_t starttime = std::clock();

  tree_dec_t tree_decomposition;
  get_nice_tree_decomposition (tree_decomposition, control_flow_graph);
  if(options.dump_graphs)
    dump_dec_lospre(tree_decomposition);

  std::clock_t midtime = std::clock();

  std::set<int>::iterator ci, ci_end;
  for (ci = candidate_set.begin(), ci_end = candidate_set.end(); ci != ci_end; ++ci)
    {
      const iCode *ic;
std::cout << "lospre for " << *ci << "\n";
      for (ic = sic; ic && ic->key != *ci; ic = ic->next);
      if (!ic || !candidate_expression (ic, operandKey))
        continue;

      setup_bbcfg_lospre_for_expression (&control_flow_graph, ic);

      bb_lospre(control_flow_graph, tree_decomposition, ic);
    }

  std::clock_t endtime = std::clock();
  std::cout << "lospre time: " << double(endtime - starttime) / CLOCKS_PER_SEC << " (" << double(midtime - starttime) / CLOCKS_PER_SEC << " for treedec / " << double(endtime - midtime) / CLOCKS_PER_SEC << " for lospre)\n";
  std::cout.flush();
}

void bb_mcpre_all (const std::set<int>& candidate_set, iCode *sic, ebbIndex *ebbi)
{
  bbcfg_mcpre_t control_flow_graph;
  create_bbcfg_mcpre (control_flow_graph, sic, ebbi);

  if(options.dump_graphs)
    dump_bbcfg_mcpre(control_flow_graph);

  std::clock_t starttime = std::clock();

  std::set<int>::iterator ci, ci_end;
  for (ci = candidate_set.begin(), ci_end = candidate_set.end(); ci != ci_end; ++ci)
    {
      const iCode *ic;
std::cout << "mcpre for " << *ci << "\n";
      for (ic = sic; ic && ic->key != *ci; ic = ic->next);
      if (!ic || !candidate_expression (ic, operandKey))
        continue;

      setup_bbcfg_mcpre_for_expression (&control_flow_graph, ic);

      bb_mcpre (control_flow_graph, ic, false);
    }

  std::clock_t endtime = std::clock();
  std::cout << "MC-PRE time: " << double(endtime - starttime) / CLOCKS_PER_SEC << "\n";
  std::cout.flush();
}

void
lospre (iCode *sic, ebbIndex *ebbi)
{
  cfg_lospre_t control_flow_graph;

  wassert (sic);

//#ifdef DEBUG_LOSPRE
  if (currFunc)
    std::cout << "mc-pre / lospre for " << currFunc->rname << "()\n";
//#endif

  create_cfg_lospre (control_flow_graph, sic, ebbi);

  if(options.dump_graphs)
    dump_cfg_lospre(control_flow_graph);

  std::set<int> candidate_set;
  get_candidate_set (&candidate_set, sic, operandKey);

  candidate_set = bb_elim_all (candidate_set, sic, ebbi);
  if(candidate_set.size() == 0)
    return;

  bb_mcpre_all (candidate_set, sic, ebbi);

  bb_lospre_all (candidate_set, sic, ebbi);

  // exit(0);
}

