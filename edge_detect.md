在找邊緣時主要會有以下兩個步驟
1. 轉成灰階+高斯模糊
2. 利用微分算出差值

對於一張圖片來說就是找二維的梯度，也就是兩個方向的差值再利用畢氏定理算出值，我們可以先從一維的來看，數值微分為
## 1. 一維差分
$$\ f'(x) = \dfrac{df(x)}{dx} \sim \dfrac{ \Delta f(x)}{ \Delta x} = \dfrac{f(x_2) - f(x_1)}{x_2 - x_1}$$

假設只跟鄰近的格點做，即 $\ \Delta x = 1 $，那麼 ***f'(x) = f(x+1) - f(x)*** 或 ***f'(x) = f(x) - f(x-1)***。但若只算前差分或後差分，就不能確定邊緣位置，所以通常會用中央差分 ***f'(x) = 0.5(f(x+1) - f(x-1))***，例如像素值為 10 10 50，那差分會得到
|   | 10 | 10 | 50 |
| ---  | ---: | ---: | ---: |
|前差分| -- | 0 | 40 |
|後差分| 0 | 40 | -- |
|中央差分| 0 | 20 | 0 |

而實務上中央差分只看數值差異不看精確值，所以就不用另外乘上 0.5。

## 2. 二維差分
二維就是一維做兩次，最後再利用畢氏定理算出中心值即可。

$$\ \nabla f(x, y) = \dfrac{\partial f(x, y)}{\partial x}\hat{x} + \dfrac{\partial f(x, y)}{\partial y}\hat{y} = f_{x}(x, y)\hat{x} + f_{y}(x, y)\hat{y} \rightarrow |\nabla f(x, y)| = \sqrt(f_{x}^{2} + f_{y}^{2})$$

所以就可以寫成一個 3*3 的矩陣

$$\begin{equation}
f_{x} = 
    \begin{bmatrix}
        \-1&0&1 \\
        \-1&0&1 \\
        \-1&0&1 \\
    \end{bmatrix}
\end{equation}$$

$$\begin{equation}
f_{y} = 
    \begin{bmatrix}
        1&1&1 \\
        0&0&0 \\
        \-1&-1&-1 \\
    \end{bmatrix}
\end{equation}$$

## 3. sobel kernel
單純差分對噪聲非常敏感。所以在差分的同時，對垂直方向加上權重（例如中間行乘以 2），相當於**差分 + 平滑**，這樣能強調中心像素，減少隨機噪聲。所以上述的矩陣改成

$$\begin{equation}
f_{x} = 
    \begin{bmatrix}
        \-1&0&1 \\
        \-2&0&2 \\
        \-1&0&1 \\
    \end{bmatrix}
\end{equation}$$

例如一張圖二值化後的數值為

$$\begin{equation}
I = 
    \begin{bmatrix}
        10&10&10&10&10 \\
        10&10&10&10&10 \\
        10&50&50&50&10 \\
        10&50&50&50&10 \\
        10&10&10&10&10 \\
    \end{bmatrix}
\end{equation}$$

那麼經過 sobel_x 計算後矩陣為

$$\begin{equation}
G_{x} = 
    \begin{bmatrix}
        0&0&0&0&0 \\
        0&120&160&120&0 \\
        0&120&160&120&0 \\
        0&0&0&0&0 \\
        0&0&0&0&0 \\
    \end{bmatrix}
\end{equation}$$

經過 sobel_Y 計算後矩陣為

$$\begin{equation}
G_{Y} = 
    \begin{bmatrix}
        0&0&0&0&0 \\
        0&0&0&0&0 \\
        0&-160&-160&-160&0 \\
        0&-120&-120&-120&0 \\
        0&0&0&0&0 \\
    \end{bmatrix}
\end{equation}$$

$$\begin{equation}
\sqrt(G_{x}^{2} + G_{y}^{2}) = 
    \begin{bmatrix}
        0&0&0&0&0 \\
        0&120&160&120&0 \\
        0&\sqrt(120^{2} +160^{2})&\sqrt(160^{2} +160^{2})&\sqrt(120^{2} +160^{2})&0 \\
        0&120&120&120&0 \\
        0&0&0&0&0 \\
    \end{bmatrix}
    =
    \begin{bmatrix}
        0&0&0&0&0 \\
        0&120&160&120&0 \\
        0&200&226&200&0 \\
        0&120&120&120&0 \\
        0&0&0&0&0 \\
    \end{bmatrix}
\end{equation}$$

## 4. canny edge
canny 找邊算法在 1968 年提出，在 sobel 的基礎上加上了 double threshold，最後再把相鄰的點連起來來避免假邊界。如果算完 sobel 的值大於較大的 threshold 則稱為強邊界，小於較小的 threshold 則非邊界，介於中間的則是弱邊界，也就是候選邊界而已，最後要看是否與強邊界相鄰。用 opencv 來實作如下
```c++
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    // 檢查是否有輸入圖片路徑
    if (argc != 2) {
        cout << "使用方式: " << argv[0] << " <圖片路徑>" << endl;
        return -1;
    }

    // 讀取圖片
    Mat src = imread(argv[1], IMREAD_COLOR);
    if (src.empty()) {
        cout << "無法讀取圖片: " << argv[1] << endl;
        return -1;
    }

    // 轉成灰階
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    // 套用高斯模糊，降低雜訊
    Mat blurred;
    GaussianBlur(gray, blurred, Size(5, 5), 1.4);

    // Canny 邊緣檢測
    Mat edges;
    double lowThreshold = 50;
    double highThreshold = 150;
    Canny(blurred, edges, lowThreshold, highThreshold);

    // 顯示結果
    imshow("原始圖片", src);
    imshow("Canny 邊緣檢測", edges);

    waitKey(0);
    return 0;
}
```
[這篇文章](https://github.com/lina-haidar/Edge-Detection-Techniques-Sobel-vs.-Canny?tab=readme-ov-file#sobel-vs-canny)可以看到使用 canny 找出來的邊比只用 sobel 找出來的清晰且明顯許多。
