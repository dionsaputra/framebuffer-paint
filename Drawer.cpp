#include "Drawer.h"
#include <queue>

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
    for (int i=0; i<wireframe.getLines().size(); i++) {
        draw_line(wireframe.getLines()[i]);
    }
}

void Drawer::erase_wireframe(Wireframe wireframe) {
    for (int i=0; i<wireframe.getLines().size(); i++) {
        erase_line(wireframe.getLines()[i]);
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