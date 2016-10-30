#ifndef DIST_H
#define DIST_H

double mindist_c(Point p, Node n);
double minmax_c(Point p, Node n);
void mindist(sqlite3_context* ctx, int nargs, sqlite3_value** values );
void minmax(sqlite3_context* ctx, int nargs, sqlite3_value** values );

#endif
