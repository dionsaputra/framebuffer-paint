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

void Drawer::draw_line(Line line) {
    vector<Point> points = line.getPoints();
    for (int i=0; i<points.size(); i++) {
        draw_point(points[i], line.getColor());
    }
}

void Drawer::erase_line(Line line) {
    Color background(0,0,0);
    draw_point(line.start(), background);
    vector<Point> points = line.getPoints();
    for (int i=0; i<points.size(); i++) {
        draw_point(points[i], background);
    }
    draw_point(line.end(), background);
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

void Drawer::draw_Map(Map map, Wireframe window){
    for (int i=0; i<map.getParts().size();i++){
        draw_wireframe(map.getPartAt(i).clippingResult(window));
        queueFloodFill(map.getPartAt(i).clippingResult(window));
    }
}

void Drawer::erase_Map(Map map){
    for (int i=0; i<map.getParts().size();i++){
        erase_wireframe(map.getPartAt(i));
        unfill_wireframe(map.getPartAt(i));
    } 
}

void Drawer::clear_window(Wireframe window) {
    for (int x=window.getTopLeft().getX(); x<=window.getBottomRight().getX(); x++) {
        for (int y=window.getTopLeft().getY(); y<=window.getBottomRight().getY(); y++) {
            Point point(x,y);
            Color color = getColor(point);
            if (!color.isEqual(window.getBorderColor()) && !color.isEqual(Color::background())) {
                erase_point(point);
            }
        }
    }
}

int global_count = 0;
void Drawer::fill_util(int x, int y, Color prevColor, Color newColor, Point topLeft, Point bottomRight, Point topLeft2, Point bottomRight2){
    global_count++;
    if (global_count > 40000){
        global_count--;
        return;
    }
    int top = topLeft.getY(); 
    int bottom = bottomRight.getY();
    int left = topLeft.getX();
    int right = bottomRight.getX();

    // Base cases 
    if (x < left || x >= right || y < top || y >= bottom) 
        return; 
    if (!getColor(Point(x,y)).isEqual(prevColor))
        return; 
  
    // Replace the color at (x, y) 
    if (is_in_frame(Point(x,y),topLeft2,bottomRight2))
        draw_point(Point(x, y), newColor);
  
    // Recur for north, east, south and west 
    if (getColor(Point(x+1,y)).isEqual(prevColor))
        fill_util(x+1,y,prevColor,newColor,topLeft,bottomRight,topLeft2,bottomRight2);
    if (getColor(Point(x-1,y)).isEqual(prevColor)) 
        fill_util(x-1,y,prevColor,newColor,topLeft,bottomRight,topLeft2,bottomRight2);
    if (getColor(Point(x,y+1)).isEqual(prevColor)) 
        fill_util(x,y+1,prevColor,newColor,topLeft,bottomRight,topLeft2,bottomRight2);
    if (getColor(Point(x,y-1)).isEqual(prevColor))
        fill_util(x,y-1,prevColor,newColor,topLeft,bottomRight,topLeft2,bottomRight2);
}

Point search_mid_point(Wireframe wireframe, Point topLeft, Point bottomRight) {

    if (topLeft.isEqual(bottomRight)) return topLeft;

    int midX = (topLeft.getX() + bottomRight.getX())/2;
    int midY = (topLeft.getY() + bottomRight.getY())/2;
    Point point(midX, midY);

    if (wireframe.is_inside(point)) {
        return point;
    } else {
        Point quad1TopLeft(midX, topLeft.getY()); Point quad1BottomRight(bottomRight.getX(), midY);
        Point quad3TopLeft(topLeft.getX(), midY); Point quad3BottomRight(midX, bottomRight.getY());

        search_mid_point(wireframe, quad1TopLeft, quad1BottomRight);
        search_mid_point(wireframe, topLeft, point);
        search_mid_point(wireframe, quad3TopLeft, quad3BottomRight);
        search_mid_point(wireframe, point, bottomRight);
    }
}

void Drawer::fill_wireframe(Wireframe wireframe, Point topLeft, Point bottomRight) {
    int top = wireframe.getTopLeft().getY(); 
    int bottom = wireframe.getBottomRight().getY(); 
    int left = wireframe.getTopLeft().getX(); 
    int right = wireframe.getBottomRight().getX();
    int flagCangkul, flagAnomali;
    
    Color prevColor = Color(0,0,0);
    Color newColor = wireframe.getFillColor();
    global_count = 0;

    Point point = wireframe.getMostTopLeftPoint();

    fill_util(point.getX() , point.getY() ,prevColor, newColor, Point(left, top), Point(right, bottom), topLeft, bottomRight);
    
    // for(int j = top; j <= bottom; j++){
    //     flagCangkul = 0;
    //     flagAnomali = 0;
    //     for(int i = left; i <= right; i++){

    //         // if(flagCangkul){
    //         if(check_neighbour(wireframe, Point(i, j), &flagAnomali)){
    //             if(flagCangkul == 1){
    //                 flagCangkul = 0;
    //             } else {
    //                 flagCangkul = 1;
    //             }
    //         }    
    //         if(flagCangkul){
    //             if (is_in_frame(Point(i,j),topLeft,bottomRight))
    //                 draw_point(Point(i, j), wireframe.getFillColor());
    //         }
    //         // } else {
    //         //     if(check_neighbour(wireframe, Point(i+1, j), &flagAnomali)){
    //         //         if(flagCangkul == 1){
    //         //             flagCangkul = 0;
    //         //         } else {
    //         //             flagCangkul = 1;
    //         //         }
    //         //     }
    //         // }
    //     }
    // }
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

// void Drawer::draw_shape(Shape shape) {

// };

// void Drawer::erase_shape(Shape shape){

// };

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