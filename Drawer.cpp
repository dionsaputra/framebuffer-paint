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

void Drawer::draw_wireframe(Wireframe wireframe) {
    int pointsSize = wireframe.getPoints().size();
    for (int i=1; i<=pointsSize; i++) {
        drawLine(wireframe.getPoints()[(i-1)%pointsSize], wireframe.getPoints()[i%pointsSize], wireframe.getBorderColor()); 
    }
}

void Drawer::erase_wireframe(Wireframe wireframe) {
    int pointsSize = wireframe.getPoints().size();
    for (int i=1; i<=pointsSize; i++) {
        drawLine(wireframe.getPoints()[(i-1)%pointsSize], wireframe.getPoints()[i%pointsSize], Color::background()); 
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
        if (wireframe.isInEnvelope(left) && getColor(left).isEqual(Color::background())) {
            draw_point(left, fillColor);
            // cout<<"insert left"<<endl;
            pointQueue.push(left);
        }

        if (wireframe.isInEnvelope(right) && getColor(right).isEqual(Color::background())) {
            draw_point(right, fillColor);
            // cout<<"insert right"<<endl;
            pointQueue.push(right);
        }

        if (wireframe.isInEnvelope(top) && getColor(top).isEqual(Color::background())) {
            draw_point(top, fillColor);
            // cout<<"insert top"<<endl;
            pointQueue.push(top);
        }

        if (wireframe.isInEnvelope(bottom) && getColor(bottom).isEqual(Color::background())) {
            draw_point(bottom, fillColor);
            // cout<<"insert bottom"<<endl;
            pointQueue.push(bottom);
        }
    }
}

void Drawer::draw_canvas(map<string,Wireframe> canvas, Wireframe window){
    for (auto itr=canvas.begin(); itr!=canvas.end();itr++){
        draw_wireframe((itr->second).clippingResult(window));
        queueFloodFill((itr->second).clippingResult(window));
    }
}

void Drawer::erase_canvas(map<string,Wireframe> canvas){
    for (auto itr=canvas.begin(); itr!=canvas.end();itr++){
        erase_wireframe(itr->second);
        unfill_wireframe(itr->second);
    } 
}
