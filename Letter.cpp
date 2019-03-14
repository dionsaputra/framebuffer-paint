#include "letter.h"

Letter::Letter(){}
Letter::~Letter();

Letter::Letter(char c, Drawer drawer, Point point, int scale, Color color) {
    long int loc = 0;
    int x,y;
    for (y = point.y; y < limit_y(scale, point.y); y++) {
        for (x = point.x; x < limit_x(scale, point.x); x++) {
            if (check_pos(drawer.vinfo, x, y) && condition(c, x, y, scale, point.x, point.y)) {
                draw_pixel(drawer, new_point(x, y), color);
            }
        }
    }
}

void Letter::word(char* word, int len, Drawer drawer, Point point, int shift, int scale, Color color) {
    int i;
    for (i=0; i<len; i++) {
        if(!isspace(word[i])){
            letter(word[i], drawer, point, scale, color);
        }
        point.x += shift;
    }
}

int Letter::limit_x(int scale, int pos_x) {
    return scale * GRID_WIDTH + pos_x;
}

int Letter::limit_y(int scale, int pos_y) {
    return scale * GRID_HEIGHT + pos_y;
}

int Letter::check_pos(struct fb_var_screeninfo vinfo, int x, int y){
    return (x < vinfo.xres) && (y < vinfo.yres);
}

int Letter::condition_A(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x; y -= pos_y;
    return y <= scale || scale*2 <= y && y <= scale*3 || x <= scale || x >= scale*4;
}

int Letter::condition_B(int x, int y, int scale, int pos_x, int pos_y) {
    int cond_x_1 = scale*1 + pos_x,
    cond_x_2 = scale*2 + pos_x,
    cond_x_4 = scale*4 + pos_x,
    cond_y_1 = scale*1 + pos_y,
    cond_y_2 = scale*2 + pos_y,
    cond_y_3 = scale*3 + pos_y,
    cond_y_4 = scale*4 + pos_y;

    return !(x > cond_x_1 && x < cond_x_4 && ((y > cond_y_1 && y < cond_y_2) || (y > cond_y_3 && y < cond_y_4)))
            && !(x > cond_x_4 && (y < cond_y_1 || y > cond_y_4 || (y > cond_y_2 && y < cond_y_3)));
}

int Letter::condition_C(int x, int y, int scale, int pos_x, int pos_y){
    x -= pos_x; y -= pos_y;
    return y <= scale || x <= scale || y >= scale*4;
}

int Letter::condition_D(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x;
    y -= pos_y;
    return (x <= scale)
            || (scale <= x && x <= scale * 4 && y <= scale)
            || (scale <= x && x <= scale * 4 && scale*4 <= y)
            || (scale*4 <= x && scale*1 <= y && y <= scale*4);
}

int Letter::condition_E(int x, int y, int scale, int pos_x, int pos_y){ 
    int cond_x_1 = scale*1 + pos_x,
    cond_x_2 = scale*2 + pos_x,
    cond_y_1 = scale*1 + pos_y,
    cond_y_2 = scale*2 + pos_y,
    cond_y_3 = scale*3 + pos_y,
    cond_y_4 = scale*4 + pos_y;
    
    return !((x > cond_x_1) && ((y > cond_y_1 && y < cond_y_2) || (y > cond_y_3 && y < cond_y_4)));     
}

int Letter::condition_F(int x, int y, int scale, int pos_x, int pos_y){ 
    int cond_x_1 = scale*1 + pos_x,
    cond_x_2 = scale*2 + pos_x,
    cond_y_1 = scale*1 + pos_y,
    cond_y_2 = scale*2 + pos_y,
    cond_y_3 = scale*3 + pos_y;
    
    return !((x > cond_x_1) && ((y > cond_y_1 && y < cond_y_2) || (y > cond_y_3)));     
}
int Letter::condition_G(int x, int y, int scale, int pos_x, int pos_y){
    x -= pos_x; y -= pos_y;
    return y <= scale || x <= scale || y >= scale*4 || x >= scale*2 && scale*2 <= y && y <= scale*3 || x >= scale*4 && y >= scale*3;
}
int Letter::condition_H(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x; y -= pos_y;
    return scale*2 <= y && y <= scale*3 || x <= scale || x >= scale*4;
}

int Letter::condition_I(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x;
    y -= pos_y;

    return y <= scale || y >= scale*4 || (scale*2 <= x && x <= scale*3 && scale <= y && y <= scale*4); 
}

int Letter::condition_K(int x, int y, int scale, int pos_x,int pos_y){
	int cond_x_2 = scale*2 + pos_x,
	cond_x_3 = scale*3 + pos_x,
	cond_x_4 = scale*4 + pos_x,
	cond_y_1 = scale*1 + pos_y,
	cond_y_2 = scale*2 + pos_y,
	cond_y_3 = scale*3 + pos_y,
	cond_y_4 = scale*4 + pos_y;

	return !( ((y<cond_y_1 || y>=cond_y_4) && x<cond_x_4 & x>cond_x_2) 
		|| (y<=cond_y_3 && y>=cond_y_2 && x>cond_x_3) 
		|| ( ((y<cond_y_2 && y>=cond_y_1) || (y>cond_y_3 && y<cond_y_4)) && ((x>cond_x_2 && x<cond_x_3)||(x>cond_x_4))) 
		);
}

int Letter::condition_L(int x, int y, int scale, int pos_x, int pos_y) {
    int cond_x = scale*1 + pos_x,
    cond_y = scale*4 + pos_y;

    return x <= cond_x || y >= cond_y;
}

int Letter::condition_M(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x; y -= pos_y;
    return x <= scale || x >= scale*4 || scale <= x && x <= scale*2 && scale <= y && y <= scale*2 || scale*2 <= x && x <= scale*3 && scale*2 <= y && y <= scale*3 || scale*3 <= x && x<= scale*4 &&  scale <= y && y <= scale*2;
}

int Letter::condition_N(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x; y -= pos_y;
    return x <= scale || x >= scale*4 || (scale <= x && x <= scale*2 && scale <= y && y <= scale*2) || (scale*2 <= x && x <= scale*3 && scale*2 <= y && y <= scale*3) || (scale*3 <= x && x <= scale*4 && scale*3 <= y && y <= scale*4);
}
int Letter::condition_O(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x; y -= pos_y;
    return y <= scale || y >= scale*4 || (x <= scale && scale <= y && y <= scale*4) || (x >= scale*4 && scale <= y && y <= scale*4);
}

int Letter::condition_P(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x; y -= pos_y;
    return  y <= scale || scale*2 <= y && y <= scale*3 || x <= scale || x >= scale*4 && y <= scale*3;
}

int Letter::condition_S(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x; y -= pos_y;
    return y <= scale || scale*2 <= y && y <= scale*3 || y >= scale*4 || x <= scale && scale <= y && y <= scale*2 || x >= scale*4 && scale*3 <= y && y <= scale*4;
}

int Letter::condition_R(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x;
    y -= pos_y;
    return x <= scale || y <= scale || scale*2 <= y && y <= scale*3 || x >= scale*4 && y <= scale*3 || scale*3 <= x && x <= scale*4 && scale*3 <= y && y <= scale*4 || scale*4 <= x && scale*4 <= y;
}

int Letter::condition_T(int x, int y, int scale, int pos_x, int pos_y) {
    int cond_x_1 = scale*2 + pos_x,
    cond_x_2 = scale*3 + pos_x,
    cond_y_1 = scale*1 + pos_y,
    cond_y_2 = scale*2 + pos_y,
    cond_y_3 = scale*3 + pos_y,
    cond_y_4 = scale*4 + pos_y;

    return !(y > cond_y_1 && (x < cond_x_1 || x > cond_x_2));

}

int Letter::condition_U(int x, int y, int scale, int pos_x, int pos_y){ 
    x -= pos_x; y -= pos_y;
    return x <= scale || x >= scale*4 || y >= scale*4;
}

int Letter::condition_Y(int x, int y, int scale, int pos_x, int pos_y) {
    int cond_x_1 = scale*1 + pos_x,
    cond_x_2 = scale*2 + pos_x,
    cond_x_3 = scale*3 + pos_x,
    cond_x_4 = scale*4 + pos_x,
    cond_y_1 = scale*1 + pos_y,
    cond_y_2 = scale*2 + pos_y,
    cond_y_3 = scale*3 + pos_y,
    cond_y_4 = scale*4 + pos_y;

    return (y <= cond_y_1 && (x <= cond_x_1 || x >= cond_x_4)) || 
    (y >= cond_y_1 && y <= cond_y_2 && ((x >= cond_x_1 && x <= cond_x_2) || (x >= cond_x_3 && x <= cond_x_4)) ||
    (y >= cond_y_2 && x >= cond_x_2 && x <= cond_x_3));
}

int Letter::condition_Z(int x, int y, int scale, int pos_x, int pos_y) {
   x -= pos_x; y -= pos_y;
   return y <= scale || y >= scale*4 || scale <= x && x <= scale*2 && y >= scale*3 || scale*2 <= x && x <= scale*3 && scale*2 <= y && y <= scale*3 || scale*3 <= x && x <= scale*4 && y <= scale*2;
}

int Letter::condition_dash(int x, int y, int scale, int pos_x, int pos_y) {
   x -= pos_x; y -= pos_y;
   return scale*2 <= y && y <= scale*3;
}

int Letter::condition_0(int x, int y, int scale, int pos_x, int pos_y) {
   x -= pos_x; y -= pos_y;
   return scale <= x && x <= scale*4 && (y <= scale || y >= scale*4) || scale <= y && y <= scale*4 && (x <= scale || x >= scale*4);
}

int Letter::condition_1(int x, int y, int scale, int pos_x, int pos_y) {
   x -= pos_x; y -= pos_y;
   return scale*2 <= x && x <= scale*3 || y <= scale && scale <= x && x <= scale*2;
}

int Letter::condition_2(int x, int y, int scale, int pos_x, int pos_y) {
   x -= pos_x; y -= pos_y;
   return (y<=scale*1 && x>=scale*1 && x<=scale*3) || (y<=scale*2 && y>=scale*1 && (x>=scale*3 && x<scale*4 || x<=scale*1)) || (y>=scale*2 && y<=scale*3 && x<=scale*3 && x>=scale*2) || (y>=scale*3 && y<=scale*4 && x<=scale*2 && x>=scale*1) || (y>=scale*4 && x<scale*4);
}

int Letter::condition_3(int x, int y, int scale, int pos_x, int pos_y) {
   int cond_x_1 = scale*1 + pos_x,
    cond_x_2 = scale*2 + pos_x,
    cond_x_3 = scale*3 + pos_x,
    cond_x_4 = scale*4 + pos_x,
    cond_y_1 = scale*1 + pos_y,
    cond_y_2 = scale*2 + pos_y,
    cond_y_3 = scale*3 + pos_y,
    cond_y_4 = scale*4 + pos_y;
   return ((y <= cond_y_1 || y >= cond_y_4) && (x >= cond_x_1 && x<= cond_x_4)) ||
   ((x <= cond_x_1 || x >= cond_x_4) && ((y >= cond_y_1 && y <= cond_y_2) || (y >= cond_y_3 && y <= cond_y_4))) ||
   ((y >= cond_y_2 && y <= cond_y_3) && (x >= cond_x_3 && x <= cond_x_4));
}

int Letter::condition_4(int x, int y, int scale, int pos_x, int pos_y) {
   x -= pos_x; y -= pos_y;
   return (x <= scale && y <= scale*3) || (y >= scale*2 && y <= scale*3 ) || (x >= scale*3 && x <= scale*4);
}

int Letter::condition_6(int x, int y, int scale, int pos_x, int pos_y) {
    x -= pos_x; y -= pos_y;
    return !(x > scale && x < scale*5 && (y > scale && y < scale*2)) && !(x > scale && x < scale*4 && (y > scale*3 && y < scale*4));
}

int Letter::condition_7(int x, int y, int scale, int pos_x, int pos_y){
    x-= pos_x; y-= pos_y;
    return(y <= scale) || ( y <= scale*2 && x >= scale*3) || (y <= scale *3 && y >= scale*2 && x >= scale*2 && x <= scale*3) || (y <= scale *4 && y >= scale*3 && x >= scale*1 && x <= scale*2) || (y>=scale*4 && x <= scale);
} 

int Letter::condition_8(int x, int y, int scale, int pos_x, int pos_y) {
    x -= pos_x; y -= pos_y;
    return !(x > scale && x < scale*4 && ((y > scale && y < scale*2) || (y > scale*3 && y < scale*4)));
}

int Letter::condition(char c, int x, int y, int scale, int pos_x, int pos_y) {
    switch (c) {
        case 'A': return condition_A(x,y,scale,pos_x,pos_y);
        case 'B': return condition_B(x,y,scale,pos_x,pos_y);
        case 'C': return condition_C(x,y,scale,pos_x,pos_y);
        case 'D': return condition_D(x,y,scale,pos_x,pos_y);
        case 'E': return condition_E(x,y,scale,pos_x,pos_y);
        case 'F': return condition_F(x,y,scale,pos_x,pos_y);
        case 'G': return condition_G(x,y,scale,pos_x,pos_y);
        case 'H': return condition_H(x,y,scale,pos_x,pos_y);
        case 'I': return condition_I(x,y,scale,pos_x,pos_y);
        case 'K': return condition_K(x,y,scale,pos_x,pos_y);
        case 'L': return condition_L(x,y,scale,pos_x,pos_y);
        case 'M': return condition_M(x,y,scale,pos_x,pos_y);
        case 'N': return condition_N(x,y,scale,pos_x,pos_y);
        case 'O': return condition_O(x,y,scale,pos_x,pos_y);
        case 'P': return condition_P(x,y,scale,pos_x,pos_y);
        case 'S': return condition_S(x,y,scale,pos_x,pos_y);
        case 'R': return condition_R(x,y,scale,pos_x,pos_y);
        case 'T': return condition_T(x,y,scale,pos_x,pos_y);
        case 'U': return condition_U(x,y,scale,pos_x,pos_y);
        case 'Y': return condition_Y(x,y,scale,pos_x,pos_y);
        case 'Z': return condition_Z(x,y,scale,pos_x,pos_y);
        case '-': return condition_dash(x,y,scale,pos_x,pos_y);
        case '0': return condition_0(x,y,scale,pos_x,pos_y);
        case '1': return condition_1(x,y,scale,pos_x,pos_y);
        case '2': return condition_2(x,y,scale,pos_x,pos_y);
        case '3': return condition_3(x,y,scale,pos_x,pos_y);
        case '4': return condition_4(x,y,scale,pos_x,pos_y);
        case '5': return condition_S(x,y,scale,pos_x,pos_y);
        case '6': return condition_6(x,y,scale,pos_x,pos_y);
        case '7': return condition_7(x,y,scale,pos_x,pos_y);
        case '8': return condition_8(x,y,scale,pos_x,pos_y);
    }
}
