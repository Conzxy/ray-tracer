# Path tracer
该(小)项目实现的不是Whitted-style的光线追踪渲染器，而是更为现代、基于渲染方程的路径追踪渲染器。
> 两者区别可参考GAMES101光线追踪相关内容。

## Features
* 支持各种`形状`(Shape)
  * `球体`(Sphere)
  * `(矩形)平面`(Rectangle plane)
  * `盒子`(Box)
* 支持`BVH`加速结构
* 支持各种`材质`（表示光线传播特性或光照模型）
  * `理想朗伯体`(Lambertian) -- 漫反射材质
  * `金属`(Metal) -- 高光材质
  * `绝缘体`(Dielectric) -- 支持折射和高光反射
  * `散射光`(Diffuse light) -- 支持单向光和双向光
* 支持各种`纹理`（表示外观）
  * 固定颜色纹理(Solid texture)
  * 图片UV纹理(Image texture)

* 支持各种`重要性采样`
  * `余弦`密度采样(Cosine density)
  * 对`矩形`采样(Rectangle density)
  * 对`球体`采样(Sphere density)
  * 混合密度采样(Mixture density)

光源在这里只是一种材质，所以任何形状都能成为光源。
<br>对光源的重要性采样需要考虑具体形状的几何特性，比如矩形适合转化积分域为面积，而球不需要用立体角更好计算。

* 支持各种`转换`(Transformation)
  * `平移`(Translation)
  * `旋转`(Rotation)
* 相机支持`景深`(Field of depth)

## TODO
- [ ] Perlin noisy
- [ ] Motion blur
- [ ] Volume rendering

## Example
采用经典的cornell box来验证全局光照效果如何。
<br>下图是以余弦密度、对球、对光源分别以1/2, 1/4, 1/4的权重作为混合密度的渲染结果(spp=1000)。
![cornellbox.png](https://s2.loli.net/2023/01/30/pgZXUAFoscPGD5i.png)

## Reference
 [[1] Peter Shirley, Steve Hollasch, Trevor David Black. Ray Tracing: In One Weekend. 2020-12-07 ](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
<br>[[2] Peter Shirley, Steve Hollasch, Trevor David Black. Ray Tracing: The Next Week. 2020-12-07 ](https://raytracing.github.io/books/RayTracingTheNextWeek.html)
<br>[[3] Peter Shirley, Steve Hollasch, Trevor David Black. Ray Tracing: The Rest of Your Life. 2020-12-07 ](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html)
<br>[[4] 闫令琪. GAMES101 Lecture 13 Ray Tracing1(Whitted-Style Ray Tracing). 2020-3-24](https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_13.pdf)
<br>[[5] 闫令琪. GAMES101 Lecture 14 Ray Tracing2(Acceleration & Radiometry). 2020-3-27](https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_14.pdf)
<br>[[6] 闫令琪. GAMES101 Lecture 15 Ray Tracing3(Light Transport & Global Illumination). 2020-3-31](https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_15.pdf)
<br>[[7] 闫令琪. GAMES101 Lecture 16 Ray Tracing4(Monte Carlo Path Tracing). 2020-4-5](https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_16.pdf)
