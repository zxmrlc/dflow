/**
 * Copyright (c) 2013 Samuel K. Gutierrez All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Base.hxx"

#include <string>
#include <vector>
#include <set>
#include <utility>
#include <map>

class Painter;

/* variable set */
typedef std::set<std::string> vset;
/* variable, label multimap */
typedef std::multimap<std::string, int> vlabmap;

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* nodes will be the basic building block of a program */
class Node {
protected:
    /* node label */
    int _plabel, _label;
    /* node depth */
    unsigned _depth;
    /* left child pointer */
    Node *l;
    /* right child pointer */
    Node *r;
    /* graph node for control flow graph */
    void *_cfgnode;
    /* set of variables */
    vset _vars;
    /* entry point */
    vlabmap _entry;
    /* exit point */
    vlabmap _exit;
    /* not */
    bool _not;

public:
    static const std::string BOGUS_VAR;

    Node(void) { this->l = NULL;
                 this->r = NULL;
                 this->_cfgnode = NULL;
                 this->_not = false;
                 this->_depth = 0;
                 this->_plabel = 0;
                 this->_label = 0; }

    virtual ~Node(void) { ; }

    virtual unsigned depth(void) const { return this->_depth; }

    virtual void depth(unsigned depth) { this->_depth = depth; }

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label) { this->_label = ++label; }
    /* bool a = annotated */
    virtual std::string str(bool a) const = 0;
    /* bool a = annotated */
    virtual void buildAST(Painter *p, void *e, bool a) const = 0;
    /* prep for cfg creation */
    virtual void cfgPrep(Painter *p) { this->cfgPrep(p); }

    virtual void *cfgnode(void) { return this->_cfgnode; }

    virtual void cfgStitch(Painter *p, void *in, void **out) {
        this->cfgStitch(p, in, out);
    }
    virtual vset getvs(void);

    unsigned nvars(void) const { return this->_vars.size(); }

    virtual void varclean(void);

    virtual void emitVars(void) const;

    vlabmap genStartSet(void) const;

    static void emitVLabSet(const vlabmap &s);

    virtual bool rdgo(const vlabmap &in, vlabmap &out);

    virtual void emitrd(void) const = 0;
};

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
class Expression : public Node {
public:
    Expression(void) : Node() { ; }

    virtual ~Expression(void) { ; }

    virtual std::string str(bool a) const = 0;

    virtual void buildAST(Painter *p, void *e, bool a) const = 0;

    void notit(void) { this->_not = !this->_not; }

    virtual void emitrd(void) const = 0;
};

/* ////////////////////////////////////////////////////////////////////////// */
class Identifier : public Expression {
private:
    std::string _id;

public:
    Identifier(void) : Expression() { ; }

    ~Identifier(void) { ; }

    Identifier(std::string id) : Expression(), _id(id) { ; }

    std::string str(bool a) const {
        std::string out;
        if (this->_not) out += "!";
        out += this->_id;
        return out;
    }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual vset getvs(void) { vset n; n.insert(this->_id); return n; }

    virtual void emitrd(void) const {
        std::cout << this->str(false);
    }
};

/* ////////////////////////////////////////////////////////////////////////// */
class Int : public Expression {
private:
    int _value;

public:
    Int(void) : Expression() { ; }

    ~Int(void) { ; }

    Int(std::string svalue) :
        Expression(), _value(Base::string2int(svalue)) { ; }

    std::string str(bool a) const { return Base::int2string(this->_value); }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void emitrd(void) const {
        std::cout << this->str(false);
    }
};

/* ////////////////////////////////////////////////////////////////////////// */
class Float : public Expression {
private:
    float _value;

public:
    Float(void) : Expression() { ; }

    ~Float(void) { ; }

    Float(std::string svalue) :
        Expression(), _value(Base::string2float(svalue)) { ; }

    std::string str(bool a) const { return Base::float2string(this->_value); }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void emitrd(void) const {
        std::cout << this->str(false);
    }
};

/* ////////////////////////////////////////////////////////////////////////// */
class Logical : public Expression {
private:
    bool _value;

public:
    Logical(void) : Expression() { ; }

    ~Logical(void) { ; }

    Logical(const std::string &svalue) : _value(Base::string2bool(svalue)) { ; }

    std::string str(bool a) const {
        std::string out;
        if (this->_not) out += "!";
        out += Base::bool2string(this->_value);
        return out;
    }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void emitrd(void) const {
        std::cout << this->str(false);
    }
};

/* ////////////////////////////////////////////////////////////////////////// */
class AssignmentExpression : public Expression {
private:

public:
    AssignmentExpression(void) { ; }

    ~AssignmentExpression(void) { ; }

    AssignmentExpression(Identifier *id, Expression *expr);

    std::string str(bool a) const;

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label) {
        this->_plabel = label;
        this->_label = ++label;
        if (this->l) this->l->label(label);
        if (this->r) this->r->label(label);
    }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void cfgPrep(Painter *p);

    virtual bool rdgo(const vlabmap &in, vlabmap &out);

    virtual void emitrd(void) const {
        std::cout << this->str(false);
    }
};

/* ////////////////////////////////////////////////////////////////////////// */
class ArithmeticExpression : public Expression {
private:
    std::string _op;

public:
    ArithmeticExpression(void) { ; }

    ~ArithmeticExpression(void) { ; }

    ArithmeticExpression(Expression *l,
                         std::string *op,
                         Expression *r);

    std::string str(bool a) const;

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label) {
        this->_label = ++label;
        if (this->l) this->l->label(label);
        if (this->r) this->r->label(label);
    }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void emitrd(void) const {
        std::cout << this->str(false);
    }
};

/* ////////////////////////////////////////////////////////////////////////// */
class LogicalExpression : public Expression {
private:
    std::string _op;

public:
    LogicalExpression(void);

    virtual ~LogicalExpression(void) { ; }

    LogicalExpression(Expression *l,
                      std::string *op,
                      Expression *r);

    std::string str(bool a) const;

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label) {
        this->_label = ++label;
        if (this->l) this->l->label(label);
        if (this->r) this->r->label(label);
    }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void cfgPrep(Painter *p);

    virtual bool rdgo(const vlabmap &in, vlabmap &out);

    virtual void emitrd(void) const {
        std::cout << this->str(false);
    }
};

/* ////////////////////////////////////////////////////////////////////////// */
class Statement : public Node {
protected:
    bool _exprStatement;
    Expression *_expr;
    std::string _meta;

public:
    /* nodes 1 */
    std::vector<void *> aNodes;
    /* nodes 2 */
    std::vector<void *> bNodes;

    Statement(void) :
        _exprStatement(false), _expr(NULL) { ; }

    virtual ~Statement(void) { ; }

    Statement(Expression *expression);

    virtual std::string str(bool a) const;

    virtual bool exprStatement(void) const { return this->_exprStatement; }

    virtual void exprStatement(bool is) { this->_exprStatement = is; }

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label) {
        this->_label = ++label;
        if (this->_expr) this->_expr->label(label);
    }

    virtual void buildAST(Painter *p, void *e, bool a) const {
        this->_expr->buildAST(p, e, a);
    }

    virtual std::string meta(void) const { return this->_meta; }

    virtual void meta(std::string m) { this->_meta = m; }

    virtual bool whilestmt(void) const { return this->_meta == "while"; }

    virtual bool ifstmt(void) const { return this->_meta == "if"; }

    virtual void cfgPrep(Painter *p) {
        this->_expr->cfgPrep(p);
        this->_cfgnode = this->_expr->cfgnode();
    }
    void cfgStitch(Painter *p, void *in, void **out);

    virtual vset getvs(void);

    virtual bool rdgo(const vlabmap &in, vlabmap &out);

    virtual void emitrd(void) const {
        Node::emitVLabSet(this->_entry);
        std::cout << "[";
        this->_expr->emitrd();
        std::cout << "] -- " << this->label() << std::endl;
        Node::emitVLabSet(this->_exit);
    }
};
typedef std::vector<Statement> Statements;
typedef std::vector<Statement *> Statementps;

/* ////////////////////////////////////////////////////////////////////////// */
class Block : public Node {
private:
    static const int ndias;
    static const std::string diaNames[];

protected:
    Statementps _statements;

public:
    Block(void) { ; }

    virtual ~Block(void) { ; }

    virtual void add(Statement *s) { this->_statements.push_back(s); }

    virtual std::string str(bool a) const;

    virtual unsigned depth(void) const { return Node::depth(); }

    virtual void depth(unsigned depth);

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label);

    virtual void buildAST(Painter *p, void *e, bool a) const;

    void drawASTs(std::string fprefix, std::string type);

    void drawCFG(std::string fprefix, std::string type);

    unsigned nstatements(void) const { return this->_statements.size(); }

    virtual void cfgPrep(Painter *p);

    virtual void cfgStitch(Painter *p, void *in, void **out);

    virtual vset getvs(void);

    void gatherVars(void) { this->_vars = this->getvs(); }

    void rdcalc(void);

    bool rdgo(const vlabmap &in, vlabmap &out);

    virtual void emitrd(void) const {
        for (Statement *s : this->_statements) {
            s->emitrd();
        }
    }
};
typedef std::vector<Block> Blocks;

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
class Skip : public Statement {
public:
    Skip(void) { ; }

    virtual ~Skip(void) { ; }

    std::string str(bool a) const;

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void cfgPrep(Painter *p);

    virtual vset getvs(void) {
        vset bogus; bogus.insert(Node::BOGUS_VAR); return bogus;
    }

    bool rdgo(const vlabmap &in, vlabmap &out);

    virtual void emitrd(void) const {
        Node::emitVLabSet(this->_entry);
        std::cout << "skip" << std::endl;
        Node::emitVLabSet(this->_exit);
    }
};

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
class IfStatement : public Statement {
private:
    Block *_exprBlock;
    Block *_ifBlock;
    Block *_elseBlock;

public:
    IfStatement(void) { ; }

    virtual ~IfStatement(void) { ; }

    IfStatement(Block *expr, Block *ifBlock, Block *elseBlock);

    std::string str(bool a) const;

    virtual unsigned depth(void) const { return this->_depth; }

    virtual void depth(unsigned depth);

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label);

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void cfgPrep(Painter *p);

    virtual void cfgStitch(Painter *p, void *in, void **out);

    virtual vset getvs(void);

    bool rdgo(const vlabmap &in, vlabmap &out);

    virtual void emitrd(void) const {
        this->_exprBlock->emitrd();
        this->_ifBlock->emitrd();
        this->_elseBlock->emitrd();
    }
};

class WhileStatement : public Statement {
private:
    Block *_exprBlock;
    Block *_bodyBlock;

public:
    WhileStatement(void) { ; }

    virtual ~WhileStatement(void) { ; }

    WhileStatement(Block *expr, Block *bodyBlock);

    std::string str(bool a) const;

    virtual unsigned depth(void) const { return this->_depth; }

    virtual void depth(unsigned depth);

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label);

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void cfgPrep(Painter *p);

    virtual void cfgStitch(Painter *p, void *in, void **out);

    virtual vset getvs(void);

    virtual bool rdgo(const vlabmap &in, vlabmap &out);

    virtual void emitrd(void) const {
        this->_exprBlock->emitrd();
        this->_bodyBlock->emitrd();
        Node::emitVLabSet(this->_exit);
    }
};

#endif
