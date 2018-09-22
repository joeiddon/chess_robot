'use strict';
let cnvs = document.getElementById('cnvs');
let ctx = document.getElementById('ctx');
cnvs.width = innerWidth; cnvs.height = innerHeight;
//distances in cm
let arm_lengths = 20;
let gripper_length = 4;

let cam = {x:0, y:-450, z:160, fov:60, yaw:0, pitch:0, roll:0};
let wireframe = 0;

let pos = [10, 20, 10]
render();

function render(){
zengine.render(get_arm(...pos), cam, cnvs, wireframe);
}

function get_arm(x,y,z){
//bottom axel is rendered at (0,0)
//bottom of the base is on the floor (z=0)
//height of ring = 8 (as haven't got scad to hand)
let trans = zengine.translate;
let xrot = zengine.x_axis_rotate;
let yrot = zengine.y_axis_rotate;
let zrot = zengine.z_axis_rotate;
let angles = get_servo_angles(x,y,z);
//any distances to do with the arm lengths are hard-coded as too much effot to parameterize
//all in all, THIS IS BAD, HACKY CODE AND NEEDS IMPROVING
//could be improved by defining "pivot points" on each part and the relations between these (angles &  which attatch to what)
//angles.bottom = 0; //Math.PI/2;
//angles.front = 0;
let faces = [].concat(model['base'].map(f=>({verts: f.verts.map(trans(-17, -22, 0)).map(zrot(3*Math.PI/2)),
                                             col:   f.col,
                                             vect:  zrot(3*Math.PI/2)(f.vect)})),
                      model['servo'].map(f=>({verts: f.verts.map(zrot(-Math.PI/2)).map(trans(-10, 17, 4)),
                                              col:   f.col,
                                              vect:  zrot(-Math.PI/2)(f.vect)})),
                      model['ring'].map(f=>({verts: f.verts.map(xrot(Math.PI)).map(trans(0, 0, 48)),
                                             col:   f.col,
                                             vect:  xrot(Math.PI)(f.vect)})),
                      model['plate'].map(f=>({verts: f.verts.map(zrot(Math.PI)).map(trans(0, 0, 48)).map(zrot(-angles.bottom)),
                                              col:    f.col,
                                              vect:   zrot(Math.PI-angles.bottom)(f.vect)})),
                      model['servo'].map(f=>({verts: f.verts.map(xrot(-Math.PI/2)).map(zrot(Math.PI)).map(trans(26.65, -51, 51.5)).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(Math.PI-angles.bottom)(xrot(-Math.PI/2)(f.vect))})),
                      model['servo'].map(f=>({verts: f.verts.map(xrot(-Math.PI/2)).map(zrot(Math.PI)).map(trans(26.65, -51, 51.5)).map(v=>({x:v.x,y:-v.y,z:v.z})).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(Math.PI-angles.bottom)((v=>({x:v.x,y:-v.y,z:v.z}))(xrot(-Math.PI/2)(f.vect)))})),
                      model['link1'].map(f=>({verts: f.verts.map(xrot(Math.PI/2)).map(yrot(-Math.PI/2-angles.front)).map(trans(9.65, -14, 61.5)).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(-angles.bottom)(yrot(-Math.PI/2-angles.front)(xrot(Math.PI/2)(f.vect)))})),
                      model['link1'].map(f=>({verts: f.verts.map(xrot(Math.PI/2)).map(yrot(-Math.PI/2-angles.front)).map(trans(9.65, 6.2, 61.5)).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(-angles.bottom)(yrot(-Math.PI/2-angles.front)(xrot(Math.PI/2)(f.vect)))})),
                      model['link3'].map(f=>({verts: f.verts.map(xrot(Math.PI/2)).map(yrot(Math.PI/2+angles.back)).map(trans(9.65, 10.1, 61.5)).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(-angles.bottom)(yrot(Math.PI/2+angles.back)(xrot(Math.PI/2)(f.vect)))})),
                      model['link4'].map(f=>({verts: f.verts.map(xrot(Math.PI/2)).map(yrot(-Math.PI/2-angles.front)).map(trans(9.65-Math.cos(angles.back)*39.4, 6.2, 61.5+Math.sin(angles.back)*39.4)).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(-angles.bottom)(yrot(-Math.PI/2-angles.front)(xrot(Math.PI/2)(f.vect)))})),
                      model['link4'].map(f=>({verts: f.verts.map(xrot(Math.PI/2)).map(yrot(-Math.PI/2-angles.front)).map(trans(-24, -25, 81.5)).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(-angles.bottom)(yrot(-Math.PI/2-angles.front)(xrot(Math.PI/2)(f.vect)))})),
                      model['link6'].map(f=>({verts: f.verts.map(xrot(-Math.PI/2)).map(yrot(-Math.PI/2+Math.atan2(20,39.4))).map(trans(9.65+Math.cos(angles.front)*200, -14, 61.5 + Math.sin(angles.front)*200)).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(-angles.bottom)(yrot(-Math.PI/2+Math.atan2(20,39.4))(xrot(-Math.PI/2)(f.vect)))})),
                      model['link5'].map(f=>({verts: f.verts.map(xrot(Math.PI/2)).map(yrot(-Math.PI/2+angles.back)).map(trans(9.65-Math.cos(angles.back)*39.4+Math.cos(angles.front)*200, 10.1, 61.5+Math.sin(angles.back)*39.4+Math.sin(angles.front)*200)).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(-angles.bottom)(yrot(-Math.PI/2+angles.back)(xrot(Math.PI/2)(f.vect)))})),
                      model['link7'].map(f=>({verts: f.verts.map(xrot(Math.PI/2)).map(yrot(-Math.PI/2+angles.back)).map(trans(9.65+Math.cos(angles.front)*200, -21.8, 61.5+Math.sin(angles.front)*200)).map(zrot(-angles.bottom)),
                                              col:   f.col,
                                              vect:  zrot(-angles.bottom)(yrot(-Math.PI/2+angles.back)(xrot(Math.PI/2)(f.vect)))})),
//                          model['link4'].map(f=>({verts: f.verts.map(xrot(Math.PI/2)).map(yrot(-Math.PI/2+angles.back)).map(trans(9.65+Math.cos(angles.front)*200, -21.8, 61.5+Math.sin(angles.front)*200)).map(zrot(-angles.bottom)),
//                                                col:   f.col,
 //                                             vect:  zrot(-angles.bottom)(yrot(-Math.PI/2+angles.back)(xrot(Math.PI/2)(f.vect)))}))
 //INSPECT THE TRIANGLE AT THE TOP - YES WE KNOW IT ISN't completely flat, but holes don't line up !?!?!?!
                      );


return faces;
}

function get_servo_angles(x,y,z){
  let d = Math.sqrt(x**2+y**2) - gripper_length;
  let A = Math.acos((d**2+z**2) / (2*arm_lengths*(d**2+z**2)**0.5));
  let Z = Math.atan2(z,d);
  let R = Math.atan2(x,y);

  let front  = A+Z;
  let back   = A-Z;
  let bottom = R+Math.PI/2;

  return {front:front, back:back, bottom:bottom};
}

