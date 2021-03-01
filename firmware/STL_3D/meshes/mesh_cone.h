#ifndef MESH_H
#define MESH_H

#define NODECOUNT 15
#define TRICOUNT 26

#define NODE(a, b) (long)(nodes[a][b])
#define EDGE(a, b) (faces[a][b])

const long nodes[NODECOUNT][3] = {
  {(long)(-14.073*PRES), (long)(11.22285*PRES), (long)(-18.0*PRES)},
  {(long)(0.0*PRES), (long)(0.0*PRES), (long)(18.0*PRES)},
  {(long)(-7.8099*PRES), (long)(16.2174*PRES), (long)(-18.0*PRES)},
  {(long)(0.0*PRES), (long)(18.0*PRES), (long)(-18.0*PRES)},
  {(long)(7.8099*PRES), (long)(16.2174*PRES), (long)(-18.0*PRES)},
  {(long)(-17.54865*PRES), (long)(4.00545*PRES), (long)(-18.0*PRES)},
  {(long)(-17.54865*PRES), (long)(-4.00545*PRES), (long)(-18.0*PRES)},
  {(long)(-14.073*PRES), (long)(-11.22285*PRES), (long)(-18.0*PRES)},
  {(long)(-7.8099*PRES), (long)(-16.2174*PRES), (long)(-18.0*PRES)},
  {(long)(0.0*PRES), (long)(-18.0*PRES), (long)(-18.0*PRES)},
  {(long)(7.8099*PRES), (long)(-16.2174*PRES), (long)(-18.0*PRES)},
  {(long)(14.073*PRES), (long)(-11.22285*PRES), (long)(-18.0*PRES)},
  {(long)(17.54865*PRES), (long)(-4.00545*PRES), (long)(-18.0*PRES)},
  {(long)(17.54865*PRES), (long)(4.0053*PRES), (long)(-18.0*PRES)},
  {(long)(14.073*PRES), (long)(11.22285*PRES), (long)(-18.0*PRES)},
};

const unsigned char faces[TRICOUNT][3] = {
  {0, 1, 2},
  {3, 1, 4},
  {5, 1, 0},
  {6, 1, 5},
  {7, 1, 6},
  {8, 1, 7},
  {9, 1, 8},
  {10, 1, 9},
  {11, 1, 10},
  {12, 1, 11},
  {13, 1, 12},
  {14, 1, 13},
  {2, 1, 3},
  {4, 1, 14},
  {3, 0, 2},
  {6, 5, 0},
  {8, 7, 6},
  {10, 9, 8},
  {12, 11, 10},
  {14, 13, 12},
  {3, 4, 14},
  {3, 6, 0},
  {10, 8, 6},
  {14, 12, 10},
  {3, 14, 6},
  {14, 10, 6},
};

#endif // MESH_H
