## 功能

### TEST

#### 色彩更鲜艳 提示信息更丰富的测试框架

相等 	`EXPECT_EQ`

不等 	`EXPECT_NE`

小于 	`EXPECT_LT`

小于等于 `EXPECT_LE`

大于 	`EXPECT_GT`

大于等于 `EXPECT_GE`

### mfunction

##### 指向函数

##### 指向函数对象

### map

#### 迭代器

++ : 找到当前节点后继

-- ：找到当前节点前驱

#### 选择树类型

##### map支持切换树的类型

提供选择的类型有 ： BRTree (红黑树) 	AVLTree (AVL树)	BTree(B树)

#### 自定义比较函数

默认为 `std::less<>`

#### insert

1. 返回值为 pair 类型 第一个元素为指向新插入节点的迭代器，第二个元素为一个表示插入是否成功的bool值

2. 支持键值插入

3. 支持变参插入

4. 支持 [] 运算符

   当树内有该键值时 [] 运算符对该键值节点所存元素进行修改

   当树内无该键值时 [] 运算符进行插入

#### erase

1. 支持键值删除
2. 支持单一迭代器删除
3. 支持范围迭代器删除

#### output

中序遍历输出整棵树

#### 迭代器遍历

支持 for 循环遍历整个map

#### 线程安全

使用了shared_mutex

## 使用方法

### TEST

`TEST {}` 函数式调用测试 TEST内可以使用六种判断宏测试值是否符合预期

`RUN_ALL_TEST()` 运行所有测试 ( 可以直接调用，也可以像谷歌测试框架一样在主函数 `return RUN_ALL_TEST()` )

### mfunction

调用方式和 STL 的 function 函数指针相同

`mfunction<bool(int, int)> p;`

### map

`map<KeyType, ValType, compare, TreeType<KeyType, ValType, compare>> m;`

默认树的类型为红黑树 `BRTree` 默认比较函数为 `std::less<KeyType>`

定义方式可以更简洁 ：`map<KeyType, ValType> m`

## Makefile

#### make 构建

- Btest	指定平衡树类型为B树
- AVLtest    指定平衡树类型为AVL树
- BRtest      指定平衡树类型为红黑树
- comp        STL map

#### make run

- 运行所有可执行文件
- 统一输出最终完成各测试函数的执行时间
  - make run target=Btest		单独执行Btest文件 输出各测试函数的具体信息
  - make run target=AVLtest	    单独执行AVLtest文件 输出各测试函数的具体信息
  - make run target=BRtest	      单独执行BRtest文件 输出各测试函数的具体信息
  - make run target=comp	        单独执行comp文件 输出各测试函数的具体信息

makefile内可修改测试样例数 T_CASE

#### make clean

- 删除由测试产生的可执行文件
