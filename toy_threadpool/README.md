## ThreadPool

### 设计模式

访问者模式

### LOG

#### 线程安全的日志打印功能类

1. `TLOG ` 		         *无提示*

2. `TLOG_Info`              *Info提示*

3. `TLOG_Debug`        *Debug提示*

4. `TLOG_Warn `          *Warn提示*

5. `TLOG_Error `         *Error提示*

#####  使用方法

`TLOG << "msg";`

### Pool

#### 线程池

##### 使用方法

1. `ThreadPool obj(int num)`   *创建拥有 num 个线程的线程池*
2. `add_task(funcname, args...)`   *线程池加入需要运行的函数 funcname(args...)*
3. `stop()`  *等待线程池运行结束*
