# 阳光岛

## 概述

	使用SDL3, glm, nlohmann-json and Tiled创建的2D游戏

## 模块

### 帧率控制模块

### 资源管理模块

### 相机模块

### 精灵模块

### 配置读取模块

### 输入管理模块

### 游戏对象和组件模块

### 变化和精灵组件

### 场景和场景管理模块

### 关卡加载模块

	设计地图时, map文件和资源文件夹textures内的资源文件在同一级目录, 而项目文件, map和textures处于同一级
	需处理路径, 路径拼接即可
	例如:
		- "image" : "Layers\/back.png"
		- 处理
		- "image" : "..\/textures\/Layers\/back.png"

		["image" : "] 统一替换为 ["image" : "..\/textures\/]

	获取layers数组并遍历对象

#### 图片层

	- 如果 type : imagelayer, 载入单一图片
		- 关注 "parallax", "repeat", "offset" 等字段
		- 创建包含 ParallaxComponent 的游戏对象

#### 瓦片层

	- 如果 type : tilelayer, 载入瓦片层
			- 总共有'地图大小'个瓦片, 放入vector容器
			- 每个瓦片包含数据: Sprite, Type(例如solid类型)
			- 可能引用多个图块集, 因此可'先载入并保存每个图块集的数据'(载入函数), 其他瓦片层以及对象层也能继续引用
			- 通过data数组中的gid查找所需信息(查找函数, 能获取自定义属性), 填充瓦片vector
			- 创建包含TileLayerComponent的游戏对象(持有瓦片vector)

#### 对象层
	
	- 如果 type : objectgroup, 载入对象层
			- 对象数据在"objects"字段中, 每个对象都创建一个游戏对象
			- 关注的要点: gid查找源图片信息, layer数据中确认目标位置(TransformComponent)信息
				- 源图片信息: 如果是"solid"类型, 则为静止障碍物, 碰撞盒大小为整个图片大小
				- 源图片信息: 如果非"solid"类型, 则
					- 碰撞盒大小, 偏移: 获取"objectgroup" --> 获取"objects" --> "x", "y", "width", "height"
					- 获取其他指定自定义属性(模板函数), 进行必要设置/组件添加. (例如tag, gravity)
				- 源图片信息: 如果有"animation"字段(自定义), 则载入动画信息并创建动画组件
					- 必要讯息(自定义): 动画名称, 每一帧的源矩阵, 帧持续时间
			- 如果没有gid, 则代表自己绘制的形状(可能是碰撞盒, 触发器等)

	- 自定义属性的获取方法: 图块集中获取"tiles" --> "properties"