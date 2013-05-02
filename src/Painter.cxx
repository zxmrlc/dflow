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

#include "Painter.hxx"
#include "Base.hxx"
#include "Constants.hxx"
#include "DFlowException.hxx"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <sstream>

#include <string.h>
#include <ctype.h>


using namespace std;

/* ////////////////////////////////////////////////////////////////////////// */
/* graphviz configury */
/* ////////////////////////////////////////////////////////////////////////// */
static char *gvconfig[] = {
    (char *)"dot",      /* use dot */
    (char *)"-Tpdf",    /* pdf output */
    (char *)"-oabc.pdf" /* output to file abc.pdf */
};

/* ////////////////////////////////////////////////////////////////////////// */
Painter::Painter(void)
{
#if 0
    PNode n, m;
    PEdge e;
#endif

    /* set up a graphviz context */
    this->gvc = gvContext();
    /* set output and layout engine */
    gvParseArgs(this->gvc, sizeof(gvconfig) / sizeof(char *), gvconfig);
    /* prep graph so nodes and edges can be added later */
    this->graph = agopen((char *)"ast", Agdirected, 0);

#if 0
    n = agnode(graph, "n", 1);
    m = agnode(graph, "m", 1);
    e = agedge(graph, n, m, 0, 1);
    /* change node props */
    agsafeset(n, "color", "red", "");
    /* label edges */
    agsafeset(e, "label", "TEST", "");
#endif
}

/* ////////////////////////////////////////////////////////////////////////// */
Painter::~Painter(void)
{
    gvFreeLayout(gvc, graph);
    agclose(graph);
    gvFreeContext(gvc);
}

/* ////////////////////////////////////////////////////////////////////////// */
void
Painter::drawAST(std::string fileName)
{
    gvLayoutJobs(gvc, graph);
    gvRenderJobs(gvc, graph);
}

/* ////////////////////////////////////////////////////////////////////////// */
PNode
Painter::newNode(Painter *p, std::string label, int i)
{
    return agnode(p->graph, (char *)label.c_str(), i);
}

PEdge
Painter::newEdge(Painter *p, PNode n1, PNode n2, int i, int j)
{
    return agedge(p->graph, n1, n2, 0, j);
}
