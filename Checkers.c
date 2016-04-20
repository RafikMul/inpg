#include <stdio.h>
00020 #include <string.h>
00021 #include <stdlib.h>
00022 #include <math.h>
00023 
00024 #include "game.h"
00025 #include "aaball.h"
00026 
00027 #define CHECKERS_CELL_SIZE 40
00028 #define CHECKERS_NUM_PIECES 4
00029 
00030 #define CHECKERS_BOARD_WID 8
00031 #define CHECKERS_BOARD_HEIT 8
00032 
00033 #define CHECKERS_WK 1
00034 #define CHECKERS_WP 2
00035 #define CHECKERS_BK 3
00036 #define CHECKERS_BP 4
00037 
00038 #define CHECKERS_ISKING(x) (x == 1 || x == 3)
00039 #define CHECKERS_ISPAWN(x) (x == 2 || x == 4)
00040 
00041 #define CHECKERS_ISWHITE(x) (x >= 1 && x <= 2)
00042 #define CHECKERS_ISBLACK(x) (x >= 3 && x <= 4)
00043 
00044 char checkers_colors[] = 
00045         {200, 200, 200, 
00046         180, 180, 180};
00047         
00048 int     checkers_init_pos[] = 
00049 {
00050          0 , 4 , 0 , 4 , 0 , 4 , 0 , 4 ,
00051          4 , 0 , 4 , 0 , 4 , 0 , 4 , 0 ,
00052          0 , 4 , 0 , 4 , 0 , 4 , 0 , 4 ,
00053          0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
00054          0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
00055          2 , 0 , 2 , 0 , 2 , 0 , 2 , 0 ,
00056          0 , 2 , 0 , 2 , 0 , 2 , 0 , 2 ,
00057          2 , 0 , 2 , 0 , 2 , 0 , 2 , 0 ,
00058 };
00059 
00060 static int checkers_max_moves = 200;
00061 
00062 
00063 void checkers_init ();
00064 int checkers_getmove (Pos *, int, int, GtkboardEventType, Player, byte **, int **);
00065 ResultType checkers_who_won (Pos *, Player, char **);
00066 byte *checkers_movegen (Pos *);
00067 ResultType checkers_eval (Pos *, Player, float *);
00068 char ** checkers_get_pixmap (int idx, int color);
00069 void checkers_reset_uistate ();
00070         
00071 Game Checkers = 
00072         { CHECKERS_CELL_SIZE, CHECKERS_BOARD_WID, CHECKERS_BOARD_HEIT, 
00073         CHECKERS_NUM_PIECES,
00074         checkers_colors, checkers_init_pos, NULL, "Checkers",
00075         checkers_init};
00076 
00077 void checkers_init ()
00078 {
00079         game_getmove = checkers_getmove;
00080         game_movegen = checkers_movegen;
00081         game_who_won = checkers_who_won;
00082         game_eval = checkers_eval;
00083         game_get_pixmap = checkers_get_pixmap;
00084         game_reset_uistate = checkers_reset_uistate;
00085         game_file_label = FILERANK_LABEL_TYPE_ALPHA;
00086         game_rank_label = FILERANK_LABEL_TYPE_NUM | FILERANK_LABEL_DESC;
00087         game_allow_flip = TRUE;
00088         game_doc_about = 
00089                 "Checkers\n"
00090                 "Two player game\n"
00091                 "Status: Partially implemented (currently unplayable)\n"
00092                 "URL: "GAME_DEFAULT_URL("checkers");
00093 }
00094 
00095 ResultType checkers_who_won (Pos *pos, Player player, char **commp)
00096 {
00097         static char comment[32];
00098         char *who_str [2] = { "white won", "black won"};
00099         int found_w = 0, found_b = 0;
00100         int i;
00101         for (i=0; i<board_wid * board_heit; i++)
00102                 if (CHECKERS_ISWHITE (pos->board[i])) found_w = 1;
00103                 else if (CHECKERS_ISBLACK (pos->board[i])) found_b = 1;
00104         if (!found_b)
00105         {
00106                 strncpy (comment, who_str[0], 31);
00107                 *commp = comment;
00108                 return RESULT_WHITE;
00109         }
00110         if (!found_w)
00111         {
00112                 strncpy (comment, who_str[1], 31);
00113                 *commp = comment;
00114                 return RESULT_BLACK;
00115         }
00116         return RESULT_NOTYET;
00117 }
00118 
00119 byte * checkers_movegen (Pos *pos)
00120 {
00121         int i, j, diffx, diffy;
00122         byte movbuf [256];
00123         byte *movlist, *mp = movbuf;
00124         byte *board = pos->board;
00125         Player player = pos->player;
00126         for (i=0; i<board_wid; i++)
00127         for (j=0; j<board_heit; j++)
00128         {
00129                 if (player == BLACK && !CHECKERS_ISBLACK(board [j * board_wid + i]))
00130                         continue;
00131                 if (player == WHITE && !CHECKERS_ISWHITE(board [j * board_wid + i]))
00132                         continue;
00133                 for (diffx = -1; diffx <= 1; diffx += 2)
00134                 for (diffy = -1; diffy <= 1; diffy += 2)
00135                 {
00136                         if (CHECKERS_ISPAWN (board [j * board_wid + i])
00137                                         &&      diffy != (player == WHITE ? 1 : -1))
00138                                 continue;
00139                         if (!ISINBOARD(i+diffx, j+diffy)) continue;
00140                         if (board [(j + diffy) * board_wid + (i + diffx)] != 0)
00141                                 continue;
00142                         *mp++ = i; *mp++ = j; *mp++ = 0;
00143                         *mp++ = i + diffx; *mp++ = j + diffy; 
00144                         if ((player == WHITE && (j + diffy) == board_heit - 1) 
00145                                         || (player == BLACK && (j + diffy) == 0))
00146                                 *mp++ = (player == WHITE ? CHECKERS_WK : CHECKERS_BK);
00147                         else
00148                                 *mp++ = board [j * board_wid + i];
00149                         *mp++ = -1;
00150                 }
00151                 for (diffx = -2; diffx <= 2; diffx += 4)
00152                 for (diffy = -2; diffy <= 2; diffy += 4)
00153                 {
00154                         int val;
00155                         if (CHECKERS_ISPAWN (board [j * board_wid + i])
00156                                         &&      diffy != (player == WHITE ? 2 : -2))
00157                                 continue;
00158                         if (!ISINBOARD(i+diffx, j+diffy)) continue;
00159                         if (board [(j + diffy) * board_wid + (i + diffx)] != 0)
00160                                 continue;
00161                         val = board [(j + diffy/2) * board_wid + i + diffx/2];
00162                         if ((player == WHITE && !CHECKERS_ISBLACK(val)) || 
00163                                         (player == BLACK && !CHECKERS_ISWHITE (val)))
00164                                 continue;
00165                         *mp++ = i; *mp++ = j; *mp++ = 0;
00166                         *mp++ = i + diffx; *mp++ = j + diffy; 
00167                         if ((player == WHITE && (j + diffy) == board_heit - 1) 
00168                                         || (player == BLACK && (j + diffy) == 0))
00169                                 *mp++ = (player == WHITE ? CHECKERS_WK : CHECKERS_BK);
00170                         else
00171                                 *mp++ = board [j * board_wid + i];
00172                         *mp++ = i + diffx/2; *mp++ = j + diffy/2; *mp++ = 0;
00173                         *mp++ = -1;
00174                 }
00175                 
00176                         
00177         }
00178         if (mp == movbuf)
00179                 *mp++ = -1;
00180         *mp++ = -2;
00181         movlist = (byte *) (malloc (mp - movbuf));
00182         memcpy (movlist, movbuf, (mp - movbuf));
00183         return movlist;
00184 }
00185 
00186 ResultType checkers_eval (Pos *pos, Player to_play, float *eval)
00187 {
00188         float sum = 0;
00189         int i, j;
00190         for (i=0; i<board_wid; i++)
00191         for (j=0; j<board_heit; j++)
00192         {
00193                 switch (pos->board [j * board_wid + i])
00194                 {
00195                         case CHECKERS_WK: sum += (5 - fabs ((i-3.5) * (j-3.5)) / 10); break;
00196                         case CHECKERS_WP: sum += (1 + j / 10.0); break;
00197                         case CHECKERS_BK: sum -= (5 - fabs ((i-3.5) * (j-3.5)) / 10); break;
00198                         case CHECKERS_BP: sum -= (1 + (board_heit - 1 - j) / 10.0); break;
00199                 }
00200         }
00201         *eval = sum;
00202         return RESULT_NOTYET;
00203 
00204 }
00205 
00206 static int oldx = -1, oldy = -1;
00207 
00208 void checkers_reset_uistate ()
00209 {
00210         oldx = -1, oldy = -1;
00211 }
00212 
00213 int checkers_getmove (Pos *pos, int x, int y, GtkboardEventType type, Player to_play, 
00214                 byte **movp, int ** rmovep)
00215 {
00216         static byte move[10];
00217         byte *mp = move;
00218         int diffx, diffy;
00219         byte *board = pos->board;
00220         if (type != GTKBOARD_BUTTON_RELEASE) return 0;
00221         if (oldx < 0)
00222         {
00223                 int val = board [y * board_wid + x];
00224                 if ((CHECKERS_ISWHITE(val) && !(to_play == WHITE)) ||
00225                 (CHECKERS_ISBLACK(val) && !(to_play == BLACK)))
00226                         return -1;
00227                 oldx = x; oldy = y;
00228                 return 0;
00229         }
00230 
00231         if (x == oldx && y == oldy)
00232         {
00233                 oldx = -1; oldy = -1; return 0;
00234         }
00235         
00236         diffx = x - oldx;
00237         if (abs (diffx) == 1) 
00238         {
00239                 diffy = y - oldy;
00240                 if (abs (diffy) != 1)
00241                 { oldx = oldy = -1; return -1;}
00242                 if (!CHECKERS_ISKING(board [oldy * board_wid + oldx])
00243                                 && diffy != (to_play == WHITE ? 1 : -1))
00244                 { oldx = oldy = -1; return -1;}
00245                 if (board [y * board_wid + x] != 0)
00246                 { oldx = oldy = -1; return -1;}
00247                 *mp++ = oldx; *mp++ = oldy; *mp++ = 0;
00248                 *mp++ = x; *mp++ = y; 
00249                 if ((to_play == WHITE && y == board_heit - 1) 
00250                                 || (to_play == BLACK && y == 0))
00251                         *mp++ = (to_play == WHITE ? CHECKERS_WK : CHECKERS_BK);
00252                 else
00253                         *mp++ = board [oldy * board_wid + oldx];
00254                 *mp++ = -1;
00255                 *movp = move;
00256                 oldx = oldy = -1;
00257                 return 1;
00258         }
00259         if (abs (diffx) == 2)
00260         {
00261                 int val;
00262                 diffy = y - oldy;
00263                 if (abs (diffy) != 2)
00264                 { oldx = oldy = -1; return -1;}
00265                 if (!CHECKERS_ISKING(board [oldy * board_wid + oldx])
00266                                 && diffy != (to_play == WHITE ? 2 : -2))
00267                 { oldx = oldy = -1; return -1;}
00268                 if (board [y * board_wid + x] != 0)
00269                 { oldx = oldy = -1; return -1;}
00270                 val = board [(y-diffy/2) * board_wid + (x-diffx/2)];
00271                 if ((!CHECKERS_ISWHITE(val) && (to_play == BLACK)) ||
00272                 (!CHECKERS_ISBLACK(val) && (to_play == WHITE)))
00273                 { oldx = oldy = -1; return -1;}
00274                 *mp++ = oldx; *mp++ = oldy; *mp++ = 0;
00275                 *mp++ = oldx+diffx/2; *mp++ = oldy+diffy/2; *mp++ = 0;
00276                 *mp++ = x; *mp++ = y; 
00277                 if ((to_play == WHITE && y == board_heit - 1)
00278                                 || (to_play == BLACK && y == 0))
00279                         *mp++ = (to_play == WHITE ? CHECKERS_WK : CHECKERS_BK);
00280                 else
00281                         *mp++ = board [oldy * board_wid + oldx];
00282                 *mp++ = -1;
00283                 *movp = move;
00284                 oldx = oldy = -1;
00285                 return 1;
00286         }
00287         { oldx = oldy = -1; return -1;}
00288 }
00289 
00290 char ** checkers_get_pixmap (int idx, int color)
00291 {
00292         int bg;
00293         int i;
00294         static char pixbuf[CHECKERS_CELL_SIZE * (CHECKERS_CELL_SIZE+1)];
00295         for(i=0, bg=0;i<3;i++) 
00296         { int col = checkers_colors[i+3]; 
00297                 if (col<0) col += 256; bg += col * (1 << (16-8*i));}
00298         return pixmap_ball_gen (CHECKERS_CELL_SIZE, pixbuf,
00299                         CHECKERS_ISWHITE(idx) ? 0xffffff : 0x0000ff, bg, 
00300                         (idx == CHECKERS_WP || idx == CHECKERS_BP) ? 8 : 12, 24);
00301 }