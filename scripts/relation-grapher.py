import argparse
import matplotlib.pyplot as plt
import networkx as nx
import subprocess
import numpy as np

parser = argparse.ArgumentParser(
                    prog='ProgramName',
                    description='What the program does',
                    epilog='Text at the bottom of help')

parser.add_argument('-r', '--root')      
parser.add_argument('-d', '--depth')
parser.add_argument('-w', '--width')



browser_path = "./bin/browser.o"
def get_data(data_fname : str, node : str, width : int) -> dict:
    width += 1
    cat_fd = subprocess.run(["cat", data_fname],
                capture_output=True,
                text=True)
    
    out_fd = subprocess.run([browser_path, node],
                input=cat_fd.stdout,
                capture_output=True,
                text=True)
    
    res_fd = subprocess.run(["head", "-n", str(width)],
                input=out_fd.stdout,
                capture_output=True,
                text=True)
    results = res_fd.stdout
    results = results.split("\n")[1:]
    res_dict = {}
    for r in results:
        if len(r) <= 1:
            continue
        split_r = r.split(" ")
        res_dict[split_r[0]] = float(split_r[1])
    return res_dict
    
def explore(data_fname : str, node : str, width : int, max_depth : int, edges : dict = None, depth : int = 1) -> dict:
    if edges == None:
        edges = {}
    if(depth > max_depth):
        return edges
    if(node in edges.keys()):
        return edges
    data = get_data(data_fname, node, width)

    edges[node] = data

    for k in data.keys():
        explore(data_fname, k, width, max_depth, edges, depth + 1)
    return edges
        
def lerp(minv, maxv, perc):
    delta = maxv - minv
    return minv + delta * perc
def color_from_weight(minw, maxw, w, cs, ce):
    cs = np.array(cs)
    ce = np.array(ce)
    delta = ce - cs
    percent = (w - minw) / (maxw - minw) 
    return cs + delta * percent

def interpolate(smin, smax, v, dmin, dmax):
    sdelta = smax - smin
    perc = (v - smin) / sdelta
    return lerp(dmin, dmax, perc)

def main():
    

    args = parser.parse_args()
    edges = explore("./outpub.txt", "@mana:schizo.vip", 10, 10)
    G = nx.Graph()

    processed = set()
    for k1 in edges.keys():
        for k2 in edges[k1].keys():
            if k2 in processed:
                continue
            v1 = edges[k1][k2]
            if not k2 in edges.keys():
                v2 = 0.0
            elif not k1 in edges[k2].keys():
                v2 = 0.0
            else:
                v2 = edges[k2][k1]
            #g.add_node(k1)
            #g.add_node(k2)
            avg = (v1 + v2) * 0.5 * 0.003
            G.add_edge(k1, k2, color="r", weight=avg)

        processed.add(k1)
    weights = nx.get_edge_attributes(G,'weight').values()
    minw = min(weights)
    maxw = max(weights)
    nodes_weights = {}
    nodes_w_list = []
    for u,v,a in G.edges(data=True):
        if u in nodes_weights.keys():
            nodes_weights[u] = 0.0
        if v in nodes_weights.keys():
            nodes_weights[v] = 0.0
        
        weight = a["weight"]
        nodes_weights[u] = nodes_weights[v] = weight
        a['color'] = color_from_weight(minw, maxw, weight, [1, 0, 0, 1], [0, 1, 0, 1])
    for n in G.nodes():
        nodes_w_list.append(interpolate(min(nodes_weights.values()), max(nodes_weights.values()), 
                                        nodes_weights[n],
                                        25,
                                        1000))
    colors = nx.get_edge_attributes(G,'color').values()
    

    pos = nx.random_layout(G)
    fig=plt.figure()
    nx.draw(G, pos, 
            edge_color=colors, 
            width=list(weights),
            with_labels=True,
            node_color='blue',
            node_size=nodes_w_list,
            font_size=7,
            font_color="white")
    fig.set_facecolor("#00000F")
    plt.show()

main()