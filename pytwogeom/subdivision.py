
from _pytwogeom import PathVector, Path



def adaptive_subdivision(path_or_pathvector,
    distance_tolerance_sq=1., angle_tolerance=0, cusp_limit=.1, recursion_limit=32
):
    subdiv = AggAdaptiveSubdivision(distance_tolerance_sq, angle_tolerance, cusp_limit, recursion_limit)

    def path_subdivision(path):
        for sb in path.to_piecewise():
            for pt in subdiv.bezier(*sb.to_bezier(4))[:-1]:
                yield pt
        yield tuple(path.final_point())

    if isinstance(path_or_pathvector, Path):
        return list(path_subdivision(path_or_pathvector))

    if isinstance(path_or_pathvector, PathVector):
        return [list(path_subdivision(path)) for path in path_or_pathvector]



class AggAdaptiveSubdivision():
    """from http://www.antigrain.com/research/adaptive_bezier/index.html"""

    from math import atan2,fabs,pi

    #------------------------------------------------------------------------
    def __init__(self,
        distance_tolerance_sq=1, angle_tolerance=0, cusp_limit=.1,
        recursion_limit=32,
        collinearity_epsilon=1e-30, tolerance_epsilon=1e-30
    ):

        self.angle_tolerance = angle_tolerance
        self.curve_recursion_limit = recursion_limit
        self.curve_collinearity_epsilon = collinearity_epsilon
        self.curve_angle_tolerance_epsilon = tolerance_epsilon

        self.distance_tolerance = distance_tolerance_sq
        self.cusp_limit = cusp_limit

    #------------------------------------------------------------------------
    def _recursive_bezier(self, x1,y1, x2, y2, x3, y3, x4, y4, level):
        if level > self.curve_recursion_limit:
            return

        # Calculate all the mid-points of the line segments
        #----------------------
        x12   = (x1   + x2  ) / 2.;
        y12   = (y1   + y2  ) / 2.;
        x23   = (x2   + x3  ) / 2.;
        y23   = (y2   + y3  ) / 2.;
        x34   = (x3   + x4  ) / 2.;
        y34   = (y3   + y4  ) / 2.;
        x123  = (x12  + x23 ) / 2.;
        y123  = (y12  + y23 ) / 2.;
        x234  = (x23  + x34 ) / 2.;
        y234  = (y23  + y34 ) / 2.;
        x1234 = (x123 + x234) / 2.;
        y1234 = (y123 + y234) / 2.;

        if level > 0: # Enforce subdivision first time
            # Try to approximate the full cubic curve by a single straight line
            #------------------
            dx = x4-x1;
            dy = y4-y1;

            d2 = abs(((x2 - x4) * dy - (y2 - y4) * dx));
            d3 = abs(((x3 - x4) * dy - (y3 - y4) * dx));

            if d2 > self.curve_collinearity_epsilon and d3 > self.curve_collinearity_epsilon:
                # Regular care
                #-----------------
                if (d2 + d3)*(d2 + d3) <= self.distance_tolerance * (dx*dx + dy*dy):
                    # If the curvature doesn't exceed the distance_tolerance value
                    # we tend to finish subdivisions.
                    #----------------------
                    if self.angle_tolerance < self.curve_angle_tolerance_epsilon:
                        self.points.append((x1234, y1234));
                        return;

                    # Angle & Cusp Condition
                    #----------------------
                    a23 = atan2(y3 - y2, x3 - x2);
                    da1 = fabs(a23 - atan2(y2 - y1, x2 - x1));
                    da2 = fabs(atan2(y4 - y3, x4 - x3) - a23);
                    if da1 >= pi: da1 = 2*pi - da1;
                    if da2 >= pi: da2 = 2*pi - da2;

                    if da1 + da2 < self.angle_tolerance:
                        # Finally we can stop the recursion
                        #----------------------
                        self.points.append((x1234, y1234));
                        return;

                    if self.cusp_limit != 0.0:
                        if da1 > self.cusp_limit:
                            self.points.append((x2, y2))
                            return

                        if da2 > self.cusp_limit:
                            self.points.append((x3, y3));
                            return

            else:
                if d2 > self.curve_collinearity_epsilon:
                    # p1,p3,p4 are collinear, p2 is considerable
                    #----------------------
                    if d2 * d2 <= self.distance_tolerance * (dx*dx + dy*dy):
                        if self.angle_tolerance < self.curve_angle_tolerance_epsilon:
                            self.points.append((x1234, y1234))
                            return

                        # Angle Condition
                        #----------------------
                        da1 = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1))
                        if da1 >= pi: da1 = 2*pi - da1

                        if da1 < self.angle_tolerance:
                            self.points.append((x2, y2))
                            self.points.append((x3, y3))
                            return

                        if self.cusp_limit != 0.0:
                            if da1 > self.cusp_limit:
                                self.points.append((x2, y2))
                                return

                elif d3 > self.curve_collinearity_epsilon:
                    # p1,p2,p4 are collinear, p3 is considerable
                    #----------------------
                    if d3 * d3 <= self.distance_tolerance * (dx*dx + dy*dy):
                        if self.angle_tolerance < self.curve_angle_tolerance_epsilon:
                            self.points.append((x1234, y1234));
                            return

                        # Angle Condition
                        #----------------------
                        da1 = fabs(atan2(y4 - y3, x4 - x3) - atan2(y3 - y2, x3 - x2))
                        if da1 >= pi: da1 = 2*pi - da1

                        if da1 < self.angle_tolerance:
                            self.points.append((x2, y2));
                            self.points.append((x3, y3));
                            return

                        if self.cusp_limit != 0.0:
                            if da1 > self.cusp_limit:
                                self.points.append((x3, y3));
                                return

                else:
                    # Collinear case
                    #-----------------
                    dx = x1234 - (x1 + x4) / 2
                    dy = y1234 - (y1 + y4) / 2
                    if dx*dx + dy*dy <= self.distance_tolerance:
                        self.points.append((x1234, y1234));
                        return

        # Continue subdivision
        #----------------------
        self._recursive_bezier(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1);
        self._recursive_bezier(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1);


    #------------------------------------------------------------------------
    def bezier(self, xy1, xy2, xy3, xy4):
        self.points = []

        self.points.append(xy1);
        self._recursive_bezier(xy1[0], xy1[1], xy2[0], xy2[1], xy3[0], xy3[1], xy4[0], xy4[1], 0);
        self.points.append(xy4);

        return self.points
