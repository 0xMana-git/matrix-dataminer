import numpy as np
from functools import partial

def lerp(minv, maxv, perc):
    delta = maxv - minv
    return minv + delta * perc

def normalize(minv, maxv, v):
    return (v - minv) / (maxv - minv)

def color_from_weight(minw, maxw, w, cs, ce):
    cs = np.array(cs)
    ce = np.array(ce)
    delta = ce - cs
    return cs + delta * normalize(minw, maxw, w)

def is_dist_valid_manhattan(minx, miny, p1 :np.ndarray , p2 :np.ndarray) -> bool:
    delta = np.abs(p2 - p1)
    if(delta[0] < minx and delta[1] < miny):
        return False
    return True

def is_all_points_valid(points, point : np.ndarray, check_func) -> bool:
    for p2 in points:
        if not check_func(point, p2):
            return False
    return True

#if it freezes on this then u should fix ur config
def random_layout_spaced(nodes, minx, miny):
    check_fn = partial(is_dist_valid_manhattan, minx, miny)
    nodes_poses = {}
    max_gen_attempts = 20
    for node in nodes:
        ctr = 0
        while(ctr < max_gen_attempts):
            pos = np.random.rand(2)
            if(is_all_points_valid(nodes_poses.values(), pos, check_fn)):
                nodes_poses[node] = pos
                break
            ctr += 1
        if (ctr >= max_gen_attempts):
            nodes_poses[node] = np.random.rand(2)
    return nodes_poses

def ease_outexpo(v : float) -> float:
    return 1 - pow(2, -10.0 * v)