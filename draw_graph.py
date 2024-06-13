import json
import networkx as nx
import matplotlib.pyplot as plt
import sys

def draw_graph(json_file):
    with open(json_file) as f:
        data = json.load(f)
    
    G = nx.Graph()
    pos = {}

    for building in data["buildings"]:
        G.add_node(building["building_id"], label=building["building_name"])
        pos[building["building_id"]] = (building["x"], building["y"])
    
    for road in data["roads"]:
        G.add_edge(road["start"], road["end"], weight=road["road_distance"], label=road["road_id"])

    labels = nx.get_edge_attributes(G, 'label')
    node_labels = nx.get_node_attributes(G, 'label')

    nx.draw(G, pos, with_labels=True, labels=node_labels, node_size=700, node_color='skyblue', font_size=10, font_weight='bold')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=labels)
    
    plt.title(data["graph_name"])
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python draw_graph.py <json_file>")
        sys.exit(1)

    json_file = sys.argv[1]
    draw_graph(json_file)
