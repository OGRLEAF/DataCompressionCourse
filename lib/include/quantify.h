#define Q_UNIFORM(V, T_SRC, T_DST) (V)>>(T_SRC - T_DST)
#define INV_Q_UNIFORM(V, T_SRC, T_DST) (V)<<(T_DST - T_SRC)