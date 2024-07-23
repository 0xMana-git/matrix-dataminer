import numpy as np
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



def ease_outexpo(v : float) -> float:
    return 1 - pow(2, -10.0 * v)