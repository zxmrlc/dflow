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

class Painter;

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* nodes will be the basic building block of a program */
class Node {
protected:
    /* node label */
    int _label;
    /* node depth */
    unsigned _depth;
    /* left child pointer */
    Node *l;
    /* right child pointer */
    Node *r;
    /* the node painter */
    Painter *painter;

public:
    Node(void) { this->l = NULL;
                 this->r = NULL;
                 this->_depth = 0;
                 this->_label = 0; }

    virtual ~Node(void) { ; }

    virtual unsigned depth(void) const { return this->_depth; }

    virtual void depth(unsigned depth) { this->_depth = depth; }

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label) { this->_label = ++label; }

    virtual std::string str(void) const = 0;

    /* bool a = annotated */
    virtual void buildAST(Painter *p, void *e, bool a) const = 0;

    virtual void *buildCFG(Painter *p) const = 0;
};

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
class Expression : public Node {
private:

public:
    Expression(void) : Node() { ; }

    virtual ~Expression(void) { ; }

    virtual std::string str(void) const = 0;

    virtual void buildAST(Painter *p, void *e, bool a) const = 0;

    virtual void *buildCFG(Painter *p) const = 0;
};

/* ////////////////////////////////////////////////////////////////////////// */
class Identifier : public Expression {
private:
    std::string _id;

public:
    Identifier(void) : Expression() { ; }

    ~Identifier(void) { ; }

    Identifier(std::string id) : Expression(), _id(id) { ; }

    std::string str(void) const { return this->_id; }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const { ; }
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

    std::string str(void) const { return Base::int2string(this->_value); }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const { ; }
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

    std::string str(void) const { return Base::float2string(this->_value); }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const { ; }
};

/* ////////////////////////////////////////////////////////////////////////// */
class Logical : public Expression {
private:
    bool _value;

public:
    Logical(void) : Expression() { ; }

    ~Logical(void) { ; }

    Logical(const std::string &svalue) : _value(Base::string2bool(svalue)) { ; }

    std::string str(void) const { return Base::bool2string(this->_value); }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const { ; }
};

/* ////////////////////////////////////////////////////////////////////////// */
class AssignmentExpression : public Expression {
private:

public:
    AssignmentExpression(void) { ; }

    ~AssignmentExpression(void) { ; }

    AssignmentExpression(Identifier *id, Expression *expr);

    std::string str(void) const;

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label) {
        this->_label = ++label;
        if (this->l) this->l->label(label);
        if (this->r) this->r->label(label);
    }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const;
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

    std::string str(void) const;

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label) {
        this->_label = ++label;
        if (this->l) this->l->label(label);
        if (this->r) this->r->label(label);
    }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const;
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

    std::string str(void) const;

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label) {
        this->_label = ++label;
        if (this->l) this->l->label(label);
        if (this->r) this->r->label(label);
    }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const { ; }
};

/* ////////////////////////////////////////////////////////////////////////// */
class Statement : public Node {
protected:
    bool _entry;
    bool _exit;
    bool _exprStatement;
    Expression *_expr;
    std::string _meta;

public:
    Statement(void) :
        _entry(false), _exit(false), _exprStatement(false), _expr(NULL) { ; }

    virtual ~Statement(void) { ; }

    Statement(Expression *expression);

    virtual std::string str(void) const;

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

    virtual void *buildCFG(Painter *p) const {
        return this->_expr->buildCFG(p);
    }

    virtual bool entry(void) const { return this->_entry; }

    virtual void entry(bool e) { this->_entry = e; }

    virtual bool exit(void) const { return this->_exit; }

    virtual void exit(bool e) { this->_exit = e; }

    virtual std::string meta(void) const { return this->_meta; }

    virtual void exit(std::string m) { this->_meta = m; }

    virtual bool whilestmt(void) const { return this->_meta == "while"; }

    virtual bool ifstmt(void) const { return this->_meta == "if"; }
};
typedef std::vector<Statement> Statements;
typedef std::vector<Statement *> Statementps;

/* ////////////////////////////////////////////////////////////////////////// */
class Block : public Node {
private:
    static const int ndias;
    static const std::string diaNames[];

public:
    /* FIXME - public bad */
    Statementps _statements;

    Block(void) { ; }

    virtual ~Block(void) { ; }

    virtual void add(Statement *s) { this->_statements.push_back(s); }

    virtual std::string str(void) const;

    virtual unsigned depth(void) const { return Node::depth(); }

    virtual void depth(unsigned depth);

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label);

    virtual void buildAST(Painter *p, void *e, bool a) const;

    void drawASTs(std::string fprefix, std::string type);

    void drawCFG(std::string fprefix, std::string type);

    unsigned nstatements(void) const { return this->_statements.size(); }

    virtual void *buildCFG(Painter *p) const;
};
typedef std::vector<Block> Blocks;

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
class Skip : public Statement {
public:
    Skip(void) { ; }

    virtual ~Skip(void) { ; }

    std::string str(void) const {
        return Base::pad(this->depth()) + "[skip]  -- " +
               Base::int2string(this->label()) + "\n";
    }

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const;
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

    std::string str(void) const;

    virtual unsigned depth(void) const { return this->_depth; }

    virtual void depth(unsigned depth);

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label);

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const;
};

class WhileStatement : public Statement {
private:
    Block *_exprBlock;
    Block *_bodyBlock;

public:
    WhileStatement(void) { ; }

    virtual ~WhileStatement(void) { ; }

    WhileStatement(Block *expr, Block *bodyBlock);

    std::string str(void) const;

    virtual unsigned depth(void) const { return this->_depth; }

    virtual void depth(unsigned depth);

    virtual int label(void) const { return this->_label; }

    virtual void label(int &label);

    virtual void buildAST(Painter *p, void *e, bool a) const;

    virtual void *buildCFG(Painter *p) const;
};

#endif
