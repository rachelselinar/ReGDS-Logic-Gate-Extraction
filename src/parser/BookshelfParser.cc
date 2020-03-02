// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Take the name prefix into account.
#define yylex   BookshelfParserlex

// First part of user declarations.
#line 5 "BookshelfParser.yy" // lalr1.cc:404
 /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>
#include <array>

/*#include "expression.h"*/


#line 49 "BookshelfParser.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "BookshelfParser.h"

// User implementation prologue.
#line 83 "BookshelfParser.yy" // lalr1.cc:412


#include "BookshelfDriver.h"
#include "BookshelfScanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.scanner->lex


#line 75 "BookshelfParser.cc" // lalr1.cc:412


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace BookshelfParser {
#line 161 "BookshelfParser.cc" // lalr1.cc:479

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  Parser::Parser (class Driver& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {}

  Parser::~Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  Parser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  Parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  Parser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  Parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  Parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  Parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  Parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  Parser::symbol_number_type
  Parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.type_get ())
    {
            case 6: // STRING

#line 79 "BookshelfParser.yy" // lalr1.cc:614
        { delete (yysym.value.strVal); }
#line 414 "BookshelfParser.cc" // lalr1.cc:614
        break;


      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    #line 43 "BookshelfParser.yy" // lalr1.cc:741
{
    // initialize the initial location object
    yyla.location.begin.filename = yyla.location.end.filename = &driver.streamname;
}

#line 552 "BookshelfParser.cc" // lalr1.cc:741

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 7:
#line 115 "BookshelfParser.yy" // lalr1.cc:859
    { driver.createCellCbk(*(yystack_[2].value.strVal)); delete (yystack_[2].value.strVal); }
#line 662 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 10:
#line 122 "BookshelfParser.yy" // lalr1.cc:859
    { driver.addPortCbk(*(yystack_[0].value.strVal)); delete (yystack_[0].value.strVal); }
#line 668 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 11:
#line 123 "BookshelfParser.yy" // lalr1.cc:859
    { driver.addPortCbk(std::to_string((yystack_[0].value.intVal))); }
#line 674 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 23:
#line 143 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), *(yystack_[6].value.strVal), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 680 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 24:
#line 144 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), *(yystack_[6].value.strVal), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 686 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 25:
#line 145 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), *(yystack_[6].value.strVal), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 692 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 26:
#line 146 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), *(yystack_[6].value.strVal), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[2].value.strVal); }
#line 698 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 27:
#line 147 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), *(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[6].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 704 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 28:
#line 148 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), *(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[6].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 710 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 29:
#line 149 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), *(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[6].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 716 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 30:
#line 150 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), *(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[6].value.strVal); delete (yystack_[2].value.strVal); }
#line 722 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 31:
#line 151 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), std::to_string((yystack_[6].value.intVal)), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[2].value.strVal); }
#line 728 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 32:
#line 152 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), std::to_string((yystack_[6].value.intVal)), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 734 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 33:
#line 153 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), std::to_string((yystack_[6].value.intVal)), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 740 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 34:
#line 154 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), std::to_string((yystack_[6].value.intVal)), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 746 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 35:
#line 155 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), std::to_string((yystack_[6].value.intVal)), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[2].value.strVal); }
#line 752 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 36:
#line 156 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), std::to_string((yystack_[6].value.intVal)), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 758 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 37:
#line 157 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), std::to_string((yystack_[6].value.intVal)), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 764 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 38:
#line 158 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addMOSCbk(*(yystack_[7].value.strVal), std::to_string((yystack_[6].value.intVal)), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[7].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 770 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 39:
#line 161 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResCbk(*(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 776 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 40:
#line 162 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResCbk(*(yystack_[4].value.strVal), *(yystack_[3].value.strVal), std::to_string((yystack_[2].value.intVal))); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); }
#line 782 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 41:
#line 163 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResCbk(*(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 788 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 42:
#line 164 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResCbk(*(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), std::to_string((yystack_[2].value.intVal))); delete (yystack_[4].value.strVal); }
#line 794 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 43:
#line 165 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResSCbk(*(yystack_[5].value.strVal), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 800 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 44:
#line 166 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResSCbk(*(yystack_[5].value.strVal), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 806 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 45:
#line 167 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResSCbk(*(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[2].value.strVal); }
#line 812 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 46:
#line 168 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResSCbk(*(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 818 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 47:
#line 169 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResNCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 824 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 48:
#line 170 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResNCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 830 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 49:
#line 171 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResNCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 836 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 50:
#line 172 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResNCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[2].value.strVal); }
#line 842 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 51:
#line 173 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResNCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 848 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 52:
#line 174 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResNCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 854 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 53:
#line 175 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResNCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 860 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 54:
#line 176 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addResNCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[2].value.strVal); }
#line 866 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 55:
#line 179 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapCbk(*(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 872 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 56:
#line 180 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapCbk(*(yystack_[4].value.strVal), *(yystack_[3].value.strVal), std::to_string((yystack_[2].value.intVal))); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); }
#line 878 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 57:
#line 181 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapCbk(*(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 884 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 58:
#line 182 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapCbk(*(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), std::to_string((yystack_[2].value.intVal))); delete (yystack_[4].value.strVal); }
#line 890 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 59:
#line 183 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapSCbk(*(yystack_[5].value.strVal), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal), delete (yystack_[2].value.strVal); }
#line 896 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 60:
#line 184 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapSCbk(*(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[2].value.strVal); }
#line 902 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 61:
#line 185 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapSCbk(*(yystack_[5].value.strVal), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 908 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 62:
#line 186 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapSCbk(*(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 914 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 63:
#line 187 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapNCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 920 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 64:
#line 188 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapNCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 926 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 65:
#line 189 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapNCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 932 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 66:
#line 190 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapNCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[2].value.strVal); }
#line 938 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 67:
#line 191 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapNCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 944 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 68:
#line 192 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapNCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 950 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 69:
#line 193 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapNCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 956 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 70:
#line 194 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCapNCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[2].value.strVal); }
#line 962 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 71:
#line 197 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addDioCbk(*(yystack_[5].value.strVal), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 968 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 72:
#line 198 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addDioCbk(*(yystack_[5].value.strVal), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 974 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 73:
#line 199 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addDioCbk(*(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 980 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 74:
#line 200 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addDioCbk(*(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[5].value.strVal); delete (yystack_[2].value.strVal); }
#line 986 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 75:
#line 203 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addBjtCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 992 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 76:
#line 204 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addBjtCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 998 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 77:
#line 205 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addBjtCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 1004 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 78:
#line 206 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addBjtCbk(*(yystack_[6].value.strVal), *(yystack_[5].value.strVal), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[5].value.strVal); delete (yystack_[2].value.strVal); }
#line 1010 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 79:
#line 207 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addBjtCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 1016 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 80:
#line 208 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addBjtCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), *(yystack_[4].value.strVal), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 1022 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 81:
#line 209 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addBjtCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), *(yystack_[3].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[3].value.strVal); delete (yystack_[2].value.strVal); }
#line 1028 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 82:
#line 210 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addBjtCbk(*(yystack_[6].value.strVal), std::to_string((yystack_[5].value.intVal)), std::to_string((yystack_[4].value.intVal)), std::to_string((yystack_[3].value.intVal)), *(yystack_[2].value.strVal)); delete (yystack_[6].value.strVal); delete (yystack_[2].value.strVal); }
#line 1034 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 83:
#line 213 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCompCbk(*(yystack_[2].value.strVal), *(yystack_[1].value.strVal)); delete (yystack_[2].value.strVal); delete (yystack_[1].value.strVal); }
#line 1040 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 84:
#line 214 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCompCbk(*(yystack_[4].value.strVal), *(yystack_[2].value.strVal)); delete (yystack_[4].value.strVal); delete (yystack_[2].value.strVal); }
#line 1046 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 85:
#line 215 "BookshelfParser.yy" // lalr1.cc:859
    {driver.addCompCbk(*(yystack_[3].value.strVal), *(yystack_[1].value.strVal)); delete (yystack_[3].value.strVal); delete (yystack_[1].value.strVal); }
#line 1052 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 88:
#line 223 "BookshelfParser.yy" // lalr1.cc:859
    { driver.addCompPortCbk(*(yystack_[0].value.strVal)); delete (yystack_[0].value.strVal); }
#line 1058 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 89:
#line 224 "BookshelfParser.yy" // lalr1.cc:859
    { driver.addCompPortCbk(std::to_string((yystack_[0].value.intVal))); }
#line 1064 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 93:
#line 232 "BookshelfParser.yy" // lalr1.cc:859
    { driver.addfeatureCbk(*(yystack_[0].value.strVal)); delete (yystack_[0].value.strVal); }
#line 1070 "BookshelfParser.cc" // lalr1.cc:859
    break;

  case 94:
#line 234 "BookshelfParser.yy" // lalr1.cc:859
    { driver.endofcellCbk(); }
#line 1076 "BookshelfParser.cc" // lalr1.cc:859
    break;


#line 1080 "BookshelfParser.cc" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char Parser::yypact_ninf_ = -32;

  const signed char Parser::yytable_ninf_ = -89;

  const short int
  Parser::yypact_[] =
  {
       0,   -32,    29,    15,    16,   -32,    43,   -32,    15,   -32,
      72,   -32,    64,   -32,    67,    70,    81,    82,    86,    89,
      39,    72,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
     -32,     3,   -32,    90,    93,    94,    97,    98,   101,   102,
     106,   109,   113,   -32,    61,   117,   -32,     0,   -32,   -32,
     -32,   -32,   121,   125,   129,   133,     4,     8,    14,    18,
      19,    22,    30,    33,    52,    55,    71,    83,   142,   143,
     146,   147,   -32,    -2,   -32,   -32,   150,   151,   154,   155,
     158,   159,   162,   163,   103,    27,   -32,     2,   -32,   108,
      32,     2,   112,    35,     2,   116,    38,     2,   120,    41,
       2,   124,    44,     2,   128,    46,     2,   132,    49,     2,
     157,   157,   157,   157,   135,   167,   170,   173,   176,   179,
     182,   185,     2,   -32,   188,   189,   190,   191,   192,   218,
     219,   220,   222,   223,   225,   226,   228,   229,   231,   232,
     157,   157,     2,   -32,   -32,   157,   157,     2,   -32,   157,
     157,     2,   -32,   157,   157,     2,   -32,   157,   157,     2,
     -32,   157,   157,     2,   -32,   157,   157,     2,   -32,   157,
     157,     2,   -32,     2,     2,     2,     2,   157,   157,   157,
     157,   157,   157,   157,   157,   -32,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,     2,     2,   -32,     2,     2,   -32,     2,     2,
     -32,     2,     2,   -32,     2,     2,   -32,     2,     2,   -32,
       2,     2,   -32,     2,     2,   -32,   -32,   -32,   -32,   -32,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   -32,   -32,   -32,   -32,   -32,   -32,
     -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
     -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
     -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
     -32,   -32,   -32,   -32
  };

  const unsigned char
  Parser::yydefact_[] =
  {
      96,    98,     0,     0,    95,     1,     0,     2,     3,     5,
      14,    97,    12,     4,     0,     0,     0,     0,     0,     0,
       0,    13,    16,    17,    18,    21,    19,    20,    22,    11,
      10,    12,     9,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    89,    96,     0,    87,    96,     6,    15,
       8,     7,     0,     0,     0,     0,    92,    92,    92,    92,
      92,    92,    92,    92,     0,     0,     0,     0,     0,     0,
       0,     0,    83,    92,    86,    94,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    92,    93,    96,    91,     0,
      92,    96,     0,    92,    96,     0,    92,    96,     0,    92,
      96,     0,    92,    96,     0,    92,    96,     0,    92,    96,
      92,    92,    92,    92,     0,     0,     0,     0,     0,     0,
       0,     0,    96,    85,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      92,    92,    96,    90,    42,    92,    92,    96,    41,    92,
      92,    96,    40,    92,    92,    96,    39,    92,    92,    96,
      58,    92,    92,    96,    57,    92,    92,    96,    56,    92,
      92,    96,    55,    96,    96,    96,    96,    92,    92,    92,
      92,    92,    92,    92,    92,    84,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    96,    96,    45,    96,    96,    46,    96,    96,
      44,    96,    96,    43,    96,    96,    60,    96,    96,    62,
      96,    96,    61,    96,    96,    59,    74,    73,    72,    71,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    54,    53,    52,    51,    50,    49,
      48,    47,    70,    69,    68,    67,    66,    65,    64,    63,
      82,    81,    80,    79,    78,    77,    76,    75,    31,    32,
      34,    33,    35,    36,    38,    37,    30,    29,    28,    27,
      26,    25,    24,    23
  };

  const short int
  Parser::yypgoto_[] =
  {
     -32,   -32,   -32,   -32,   233,   -32,   -32,   209,   -32,   -32,
     227,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   198,   333,
     136,   -32,   -31,   -32
  };

  const signed char
  Parser::yydefgoto_[] =
  {
      -1,     2,     7,     8,     9,    10,    31,    32,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    45,    46,    87,
      88,    48,     3,     4
  };

  const short int
  Parser::yytable_[] =
  {
      51,     1,   -88,     1,   -88,     1,     1,    29,    84,    30,
      85,    86,    89,    72,    90,    86,    75,    86,    92,    11,
      93,    86,    95,    98,    96,    99,   101,    86,   102,     5,
       6,    86,    86,   141,   104,    86,   105,   107,   146,   108,
      86,   150,   123,    86,   154,    86,    86,   158,    86,    12,
     162,    86,   166,    47,    86,   170,   144,    86,   110,    86,
     148,   111,    86,   152,     1,   -88,   156,   -88,    29,   160,
      30,    33,   164,    34,    35,   168,    36,   112,   172,    14,
      15,    16,    17,    18,    19,    37,    39,    38,    40,   113,
      41,   185,    42,    43,    52,    44,    53,    54,    56,    55,
      57,    58,    60,    59,    61,    62,    64,    63,    65,   140,
      66,   204,    67,    68,   145,    69,   207,    70,   149,    71,
     210,    43,   153,    73,   213,    76,   157,    77,   216,    78,
     161,    79,   219,    80,   165,    81,   222,    82,   169,    83,
     225,   177,   226,   227,   228,   229,   114,   116,   115,   117,
     118,   120,   119,   121,   124,   126,   125,   127,   128,   130,
     129,   131,   132,   134,   133,   135,   136,   138,   137,   139,
      86,   254,   255,   178,   256,   257,   179,   258,   259,   180,
     260,   261,   181,   262,   263,   182,   264,   265,   183,   266,
     267,   184,   268,   269,   186,   187,   188,   189,   190,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   143,   191,   192,   193,   143,   194,   195,
     143,   196,   197,   143,   198,   199,   143,   200,   201,   143,
      50,    13,   143,    74,     0,   143,     0,     0,    49,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   143,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   143,     0,
       0,     0,     0,   143,     0,     0,     0,   143,     0,     0,
       0,   143,     0,     0,     0,   143,     0,     0,     0,   143,
       0,     0,     0,   143,     0,     0,     0,   143,     0,   143,
     143,   143,   143,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   143,   143,
       0,   143,   143,     0,   143,   143,     0,   143,   143,     0,
     143,   143,     0,   143,   143,     0,   143,   143,     0,   143,
     143,     0,     0,     0,     0,     0,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
      91,    94,    97,   100,   103,   106,   109,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   122,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   142,     0,
       0,     0,     0,   147,     0,     0,   151,     0,     0,   155,
       0,     0,   159,     0,     0,   163,     0,     0,   167,     0,
       0,   171,     0,   173,   174,   175,   176,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   202,   203,     0,     0,     0,   205,   206,
       0,     0,   208,   209,     0,     0,   211,   212,     0,     0,
     214,   215,     0,     0,   217,   218,     0,     0,   220,   221,
       0,     0,   223,   224,     0,     0,     0,     0,     0,     0,
     230,   231,   232,   233,   234,   235,   236,   237,     0,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253
  };

  const short int
  Parser::yycheck_[] =
  {
      31,     3,     4,     3,     6,     3,     3,     4,     4,     6,
       6,    13,     4,    44,     6,    13,    47,    13,     4,     3,
       6,    13,     4,     4,     6,     6,     4,    13,     6,     0,
      15,    13,    13,     6,     4,    13,     6,     4,     6,     6,
      13,     6,    73,    13,     6,    13,    13,     6,    13,     6,
       6,    13,     6,    14,    13,     6,    87,    13,     6,    13,
      91,     6,    13,    94,     3,     4,    97,     6,     4,   100,
       6,     4,   103,     6,     4,   106,     6,     6,   109,     7,
       8,     9,    10,    11,    12,     4,     4,     6,     6,     6,
       4,   122,     6,     4,     4,     6,     6,     4,     4,     6,
       6,     4,     4,     6,     6,     4,     4,     6,     6,     6,
       4,   142,     6,     4,     6,     6,   147,     4,     6,     6,
     151,     4,     6,     6,   155,     4,     6,     6,   159,     4,
       6,     6,   163,     4,     6,     6,   167,     4,     6,     6,
     171,     6,   173,   174,   175,   176,     4,     4,     6,     6,
       4,     4,     6,     6,     4,     4,     6,     6,     4,     4,
       6,     6,     4,     4,     6,     6,     4,     4,     6,     6,
      13,   202,   203,     6,   205,   206,     6,   208,   209,     6,
     211,   212,     6,   214,   215,     6,   217,   218,     6,   220,
     221,     6,   223,   224,     6,     6,     6,     6,     6,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,    87,     6,     6,     6,    91,     6,     6,
      94,     6,     6,    97,     6,     6,   100,     6,     6,   103,
      31,     8,   106,    45,    -1,   109,    -1,    -1,    21,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,    -1,
      -1,    -1,    -1,   147,    -1,    -1,    -1,   151,    -1,    -1,
      -1,   155,    -1,    -1,    -1,   159,    -1,    -1,    -1,   163,
      -1,    -1,    -1,   167,    -1,    -1,    -1,   171,    -1,   173,
     174,   175,   176,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   202,   203,
      -1,   205,   206,    -1,   208,   209,    -1,   211,   212,    -1,
     214,   215,    -1,   217,   218,    -1,   220,   221,    -1,   223,
     224,    -1,    -1,    -1,    -1,    -1,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
      57,    58,    59,    60,    61,    62,    63,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    -1,    -1,    90,    -1,    -1,    93,    -1,    -1,    96,
      -1,    -1,    99,    -1,    -1,   102,    -1,    -1,   105,    -1,
      -1,   108,    -1,   110,   111,   112,   113,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   140,   141,    -1,    -1,    -1,   145,   146,
      -1,    -1,   149,   150,    -1,    -1,   153,   154,    -1,    -1,
     157,   158,    -1,    -1,   161,   162,    -1,    -1,   165,   166,
      -1,    -1,   169,   170,    -1,    -1,    -1,    -1,    -1,    -1,
     177,   178,   179,   180,   181,   182,   183,   184,    -1,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201
  };

  const unsigned char
  Parser::yystos_[] =
  {
       0,     3,    17,    38,    39,     0,    15,    18,    19,    20,
      21,     3,     6,    20,     7,     8,     9,    10,    11,    12,
      24,    25,    26,    27,    28,    29,    30,    31,    32,     4,
       6,    22,    23,     4,     6,     4,     6,     4,     6,     4,
       6,     4,     6,     4,     6,    33,    34,    14,    37,    26,
      23,    38,     4,     6,     4,     6,     4,     6,     4,     6,
       4,     6,     4,     6,     4,     6,     4,     6,     4,     6,
       4,     6,    38,     6,    34,    38,     4,     6,     4,     6,
       4,     6,     4,     6,     4,     6,    13,    35,    36,     4,
       6,    35,     4,     6,    35,     4,     6,    35,     4,     6,
      35,     4,     6,    35,     4,     6,    35,     4,     6,    35,
       6,     6,     6,     6,     4,     6,     4,     6,     4,     6,
       4,     6,    35,    38,     4,     6,     4,     6,     4,     6,
       4,     6,     4,     6,     4,     6,     4,     6,     4,     6,
       6,     6,    35,    36,    38,     6,     6,    35,    38,     6,
       6,    35,    38,     6,     6,    35,    38,     6,     6,    35,
      38,     6,     6,    35,    38,     6,     6,    35,    38,     6,
       6,    35,    38,    35,    35,    35,    35,     6,     6,     6,
       6,     6,     6,     6,     6,    38,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,    35,    35,    38,    35,    35,    38,    35,    35,
      38,    35,    35,    38,    35,    35,    38,    35,    35,    38,
      35,    35,    38,    35,    35,    38,    38,    38,    38,    38,
      35,    35,    35,    35,    35,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    38,    38,    38,    38,    38,    38,
      38,    38,    38,    38,    38,    38,    38,    38,    38,    38,
      38,    38,    38,    38,    38,    38,    38,    38,    38,    38,
      38,    38,    38,    38,    38,    38,    38,    38,    38,    38,
      38,    38,    38,    38
  };

  const unsigned char
  Parser::yyr1_[] =
  {
       0,    16,    17,    18,    19,    19,    20,    21,    22,    22,
      23,    23,    23,    24,    24,    25,    25,    26,    26,    26,
      26,    26,    26,    27,    27,    27,    27,    27,    27,    27,
      27,    27,    27,    27,    27,    27,    27,    27,    27,    28,
      28,    28,    28,    28,    28,    28,    28,    28,    28,    28,
      28,    28,    28,    28,    28,    29,    29,    29,    29,    29,
      29,    29,    29,    29,    29,    29,    29,    29,    29,    29,
      29,    30,    30,    30,    30,    31,    31,    31,    31,    31,
      31,    31,    31,    32,    32,    32,    33,    33,    34,    34,
      35,    35,    35,    36,    37,    38,    38,    39,    39
  };

  const unsigned char
  Parser::yyr2_[] =
  {
       0,     2,     2,     1,     2,     1,     3,     4,     2,     1,
       1,     1,     0,     1,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     8,     5,
       5,     5,     5,     6,     6,     6,     6,     7,     7,     7,
       7,     7,     7,     7,     7,     5,     5,     5,     5,     6,
       6,     6,     6,     7,     7,     7,     7,     7,     7,     7,
       7,     6,     6,     6,     6,     7,     7,     7,     7,     7,
       7,     7,     7,     3,     5,     4,     2,     1,     1,     1,
       2,     1,     0,     1,     2,     1,     0,     2,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "ENDL", "INT", "DOUBLE",
  "STRING", "MOS", "RES", "CAP", "DIO", "BJT", "COMP", "FEATURE",
  "KWD_ENDS", "KWD_SUBCKT", "$accept", "top", "sub_top", "subckt_blocks",
  "subckt_block", "subckt_header", "ports", "port", "subckt_lines",
  "definitions", "def", "mos_def", "res_def", "cap_def", "dio_def",
  "bjt_def", "comp_def", "comp_ports", "comp_port", "features", "feature",
  "subckt_footer", "ENDL_STAR", "ENDLS", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  Parser::yyrline_[] =
  {
       0,    99,    99,   102,   106,   107,   110,   115,   118,   119,
     122,   123,   124,   127,   128,   131,   132,   135,   136,   137,
     138,   139,   140,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   197,   198,   199,   200,   203,   204,   205,   206,   207,
     208,   209,   210,   213,   214,   215,   219,   220,   223,   224,
     227,   228,   229,   232,   234,   237,   238,   241,   242
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  Parser::token_number_type
  Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15
    };
    const unsigned int user_token_number_max_ = 270;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // BookshelfParser
#line 1720 "BookshelfParser.cc" // lalr1.cc:1167
#line 247 "BookshelfParser.yy" // lalr1.cc:1168
 /*** Additional Code ***/

void BookshelfParser::Parser::error(const Parser::location_type& l, const std::string& m)
{
    driver.error(l, m);
    exit(1);
}
