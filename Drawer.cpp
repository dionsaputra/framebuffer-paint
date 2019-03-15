#include "Drawer.h"
#include <queue>

vector<Point> bressenham(Point start, Point end);
Drawer::Drawer() {
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer size");
        exit(1);
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    screen_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel/8;
    fbp = (char *)mmap(0, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
}

Drawer::~Drawer() {
    munmap(fbp, screen_size);
    close(fbfd);
}

void Drawer::clear_screen() {
    for (int i=0; i<vinfo.xres; i++) {
        for (int j=0; j<vinfo.yres; j++) {
            Point point(i,j);
            erase_point(point);
        }
    }
}

bool Drawer::is_in_frame(Point point, Point topLeft, Point bottomRight) {
    if ((bottomRight.getX() == -1) && (bottomRight.getY() == -1)){
        return 0 <= point.getX() && point.getX() <= vinfo.xres && 0 <= point.getY() && point.getY() <= vinfo.yres;
    }else{
        return topLeft.getX() <= point.getX() && point.getX() <= bottomRight.getX() && topLeft.getY() <= point.getY() && point.getY() <= bottomRight.getY();
    } 
}

long int Drawer::location(Point point) {
    int x = point.getX(), y = point.getY();
    return (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
}

void Drawer::draw_point(Point point, Color color) {
    if (is_in_frame(point)) {
        long int loc = location(point);
        *(fbp+loc) = color.getBlue();
        *(fbp+loc+1) = color.getGreen();
        *(fbp+loc+2) = color.getRed();
        *(fbp+loc+3) = 0;
    }
}

void Drawer::drawLine(Point start, Point end, Color color) {
    int x1,x2,y1,y2;
    draw_point(start,color);

    if (abs(end.getX()-start.getX()) >= abs(end.getY()-start.getY())){
    	if (start.getX() <= end.getX()){
    		x1=start.getX();
	    	x2=end.getX();
	    	y1=start.getY();
	    	y2=end.getY();
    	}else{
    		x1=end.getX();
    		x2=start.getX();
    		y1=end.getY();
    		y2=start.getY();
    	}
    }else{
    	if (start.getY() <= end.getY()){
    		x1=start.getY();
    		x2=end.getY();
    		y1=start.getX();
    		y2=end.getX();
    	}else{
    		x1=end.getY();
    		x2=start.getY();
    		y1=end.getX();
    		y2=start.getX();
    	}
    }

    int dx = x2-x1;
    int dy = y2-y1;
    int turun = 1;

    if (y2 < y1){
        dy = y1 - y2;
        turun = 0;
    }

    int D = 2*dy - dx;

    for(int x = x1+1, y = y1; x < x2; x++){
        
        if (D > 0){
            if (turun){
                y++;
            }else{
                y--;
            }
            D -= 2*dx;
        }
        D += 2*dy;
        if (abs(end.getX()-start.getX()) >= abs(end.getY()-start.getY())){
            draw_point(Point(x,y), color);
        } else{
            draw_point(Point(y,x), color);
        }
    } 
}

void Drawer::drawLineWidth(Point pointStart, Point pointEnd, float wd, Color color, char style)
{
    int x0 = pointStart.getX();
    int x1 = pointEnd.getX();
    int y0 = pointStart.getY();
    int y1 = pointEnd.getY();

    if(y0 == y1 && x0 < x1){
        x0 -= (wd/2);
    }

    int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1; 
    int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1; 
    int err = dx-dy, e2, x2, y2;                          /* error value e_xy */
    float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);
    
    /* pixel loop */
    for (wd = (wd+1)/2; ; ) {            
        draw_point(Point(x0,y0), color);
        e2 = err; x2 = x0;
        if (2*e2 >= -dx) {                                           /* x step */
            for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx){
                if ((style == 's' ) || (style == 'd' && (x0%30) <= 15)){
                    draw_point(Point(x0,y2 += sy), color);
                }
            }
            if (x0 == x1) break;
            e2 = err; err -= dy; x0 += sx; 
        } 
        if (2*e2 <= dy) {                                            /* y step */
            for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy){
                if ((style == 's') || (style == 'd' && (x0%30) <= 15)){
                    draw_point(Point(x2 + sx,y0), color);
                }
            }
            if (y0 == y1) break;
            err += dx; y0 += sy; 
        }
    }
}


Color Drawer::getColor(Point point) {
    if (is_in_frame(point)) {
        Color color;
        long int loc = location(point);
        color.setBlue(*(fbp+loc) & 0xff);
        color.setGreen(*(fbp+loc+1) & 0xff);
        color.setRed(*(fbp+loc+2) & 0xff);
        return color;
    }
    return Color::background();
}

void Drawer::erase_point(Point point) {
    draw_point(point, Color::background());
}

void Drawer::draw_wireframe(Wireframe wireframe,bool useStyle) {
    int pointsSize = wireframe.getPoints().size();
    for (int i=1; i<=pointsSize; i++) {
        // drawLine(wireframe.getPoints()[(i-1)%pointsSize], wireframe.getPoints()[i%pointsSize], wireframe.getBorderColor());
        drawLineWidth(wireframe.getPoints()[(i-1)%pointsSize], wireframe.getPoints()[i%pointsSize], wireframe.getThickness(), wireframe.getBorderColor(), wireframe.getLineStyle()); 
    }
}

void Drawer::erase_wireframe(Wireframe wireframe) {
    int pointsSize = wireframe.getPoints().size();
    for (int i=1; i<=pointsSize; i++) {
        drawLineWidth(wireframe.getPoints()[(i-1)%pointsSize], wireframe.getPoints()[i%pointsSize], wireframe.getThickness() ,Color::background()); 
    }
}

void Drawer::unfill_wireframe(Wireframe wireframe){
    int top = wireframe.getTopLeft().getY(); 
    int bottom = wireframe.getBottomRight().getY(); 
    int left = wireframe.getTopLeft().getX(); 
    int right = wireframe.getBottomRight().getX();

    for(int j = top; j <= bottom; j++){
        for(int i = left; i <= right; i++)
            draw_point(Point(i, j), Color(1,1,1));
    }
}

void Drawer::queueFloodFill(Wireframe wireframe) {
    Point startPoint = wireframe.getInnerPoint();
    Color fillColor = wireframe.getFillColor();
    Color borderColor = wireframe.getBorderColor();
    draw_point(startPoint, fillColor);
    queue<Point> pointQueue;
    pointQueue.push(startPoint);
    Point nextPoint;
    while (!pointQueue.empty()) {
        nextPoint = pointQueue.front();
        pointQueue.pop();

        // for (int i=0; i<100000; i++);
        Point left = nextPoint.getLeft();
        Point right = nextPoint.getRight();
        Point top = nextPoint.getTop();
        Point bottom = nextPoint.getBottom();

        // getColor(left).display();
        if (wireframe.isInEnvelope(left) && !getColor(left).isEqual(borderColor) && !getColor(left).isEqual(fillColor)) {
            draw_point(left, fillColor);
            // cout<<"insert left"<<endl;
            pointQueue.push(left);
        }

        if (wireframe.isInEnvelope(right) && !getColor(right).isEqual(borderColor) && !getColor(right).isEqual(fillColor)) {
            draw_point(right, fillColor);
            // cout<<"insert right"<<endl;
            pointQueue.push(right);
        }

        if (wireframe.isInEnvelope(top) && !getColor(top).isEqual(borderColor) && !getColor(top).isEqual(fillColor)) {
            draw_point(top, fillColor);
            // cout<<"insert top"<<endl;
            pointQueue.push(top);
        }

        if (wireframe.isInEnvelope(bottom) && !getColor(bottom).isEqual(borderColor) && !getColor(bottom).isEqual(fillColor)) {
            draw_point(bottom, fillColor);
            // cout<<"insert bottom"<<endl;
            pointQueue.push(bottom);
        }
    }
}

void Drawer::draw_canvas(map<string,Wireframe> canvas, Wireframe window, Point disorientasi, bool useStyle){
    for (auto itr=canvas.begin(); itr!=canvas.end();itr++){
        Wireframe wireframe = itr->second;
        wireframe.translate(disorientasi.getX(),disorientasi.getY());

        Wireframe clippingWireframe = wireframe.clippingResult(window);
        if (clippingWireframe.getPoints().size() > 0) {
            draw_wireframe(clippingWireframe);
            queueFloodFill(clippingWireframe);
        }
    }
    if (useStyle){
        for (auto itr=canvas.begin(); itr!=canvas.end();itr++){
            Wireframe wireframe = itr->second;
            erase_wireframe(wireframe);
        }
        for (auto itr=canvas.begin(); itr!=canvas.end();itr++){
            Wireframe wireframe = itr->second;
            wireframe.translate(disorientasi.getX(),disorientasi.getY());
            draw_wireframe(wireframe.clippingResult(window));
        }
    }
}

void Drawer::erase_canvas(map<string,Wireframe> canvas, Point disorientasi){
    for (auto itr=canvas.begin(); itr!=canvas.end();itr++){
        Wireframe wireframe = itr->second;
        wireframe.translate(disorientasi.getX(),disorientasi.getY());
        erase_wireframe(wireframe);
        unfill_wireframe(wireframe);
    } 
}

void Drawer::draw_letter(Letter letter) {
    int minX = letter.getStartPoint().getX();
    int minY = letter.getStartPoint().getY();
    for (int y = minY; y <= letter.getLimitY(); y++) {
        for (int x = minX; x <= letter.getLimitX(); x++) {
            Point point(x,y);
            if (letter.condition(point)) {
                draw_point(point, letter.getColor());
            }
        }
    }
}

void Drawer::draw_word(string word, Point startPoint, int shift, int scale, Color color){
    int i;
    for (i=0; i < word.length(); i++) {
        if(!isspace(word[i])){
            draw_letter(Letter(word[i], startPoint, scale, color));
        }
        startPoint.setX(startPoint.getX() + shift);
    }
}