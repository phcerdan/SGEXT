import pydot
filePath = '/home/phc/repository_local/dgtal-play/src/python-scripts/visualize_dot/airway_p4.dot'
graphs = pydot.graph_from_dot_file(filePath)
graph = graphs[0]



