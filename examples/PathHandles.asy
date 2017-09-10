

void draw_path_handles(path p, pen stroke=purple, pen fill=white, real markersize=1){
    for(int i = 0; i <= length(p); ++i){
        pair a = precontrol(p, i);
        pair b = point(p, i);
        pair c = postcontrol(p, i);

        draw(a--b--c, stroke);

        path square = scale(markersize)*shift(-0.5,-0.5)*unitsquare;
        path circle = scale(0.5*markersize)*unitcircle;

        if(a==b && b==c)
            filldraw(shift(b)*square, fill, stroke+solid);
        else{
            draw(a--b--c, stroke);

            if(a!=b)
                filldraw(shift(a)*circle, fill, stroke+solid);
            if(c!=b)
                filldraw(shift(c)*circle, fill, stroke+solid);

            filldraw(shift(b)*rotate(45)*square, fill, stroke+solid);

        }
    }
}


void draw_path_handles(path ps[], pen stroke=purple, pen fill=white, real markersize=1){
    for(int i = 0; i < ps.length; ++i)
        draw_path_handles(ps[i], stroke, fill, markersize);
}


void draw_path_handles(guide g, pen stroke=purple, pen fill=white, real markersize=1){
    draw_path_handles((path)g, stroke, fill, markersize);
}
