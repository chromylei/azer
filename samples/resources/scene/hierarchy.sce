
[
  {
    "name": "scene1",
    "position": [0, 0, 0],
    "type": "scene",
    "children": [
      ":joint1",
    ],
  },

  {
    "name": "joint1",
    "position": [0, 0, 0],
    "type": "scene",
    "children": [
      ":sphere1", ":bone1", ":joint2",
    ],
  },

  {
    "name": "joint2",
    "position": [0, 0, -4],
    "type": "scene",
    "children": [
      ":sphere2", ":bone2", ":joint3",
    ],
  },

  {
    "name": "joint3",
    "position": [0, 0, -4],
    "type": "scene",
    "children": [
      ":sphere3",
    ],
  },

  {
    "name": "sphere1",
    "type": "mesh",
    "position": [0.0, 0.0, 0.0],
    "scale": [0.25, 0.25, 0.25],
    "orientation": [1, 0, 0, 0],
    "attach_object": "mesh/sphere.afxm",
  },

  {
    "name": "sphere2",
    "type": "mesh",
    "position": [0.0, 0.0, 0.0],
    "scale": [0.25, 0.25, 0.25],
    "orientation": [1, 0, 0, 0],
    "attach_object": "mesh/sphere1.afxm",
  },

  {
    "name": "sphere3",
    "type": "mesh",
    "position": [0.0, 0.0, 0.0],
    "scale": [0.25, 0.25, 0.25],
    "orientation": [1, 0, 0, 0],
    "attach_object": "mesh/sphere2.afxm",
  },

  {
    "name": "bone1",
    "type": "mesh",
    "position": [0.0, 0.0, -2.0],
    "scale": [0.1, 0.1, 2.0],
    "orientation": [1, 0, 0, 0],
    "attach_object": "mesh/bone.afxm",
  },

  {
    "name": "bone2",
    "type": "mesh",
    "position": [0.0, 0.0, -2.0],
    "scale": [0.1, 0.1, 2.0],
    "orientation": [1, 0, 0, 0],
    "attach_object": "mesh/bone.afxm",
  },
]
