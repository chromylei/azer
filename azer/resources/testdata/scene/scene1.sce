
[
  {
    "name": "scene1",
    "position" : [1.0, 5.0, 8.0],
    "orientation": [1, 1, 1, 1],
    "type": "scene",
    "children" : [
      ":node1",
      ":node2",
    ],
  },

  {
    "name": "node1",
    "position" : [1.0, 10.0, 3.0],
    "orientation": [1, 1, 1, 1],
    "attach_object": "",
    "type": "subscene",
    "children" : [
      ":node11"
    ],
  },

  {
    "name": "node2",
    "position" : [1.0, 2.0, 3.0],
    "orientation": [1, 1, 1, 1],
    "attach_object": "",
    "type": "mesh",
    "children" : [
    ],
  },

  {
    "name": "node11",
    "position" : [1.0, 5.0, 8.0],
    "orientation": [1, 1, 1, 1],
    "type": "renderable",
    "attach_object": "",
  }
]