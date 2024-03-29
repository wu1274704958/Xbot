#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/functions.hpp>
#include <gflags/gflags.h>

DEFINE_string(img,"C:\\Users\\Administrator\\Pictures\\dra.png","Input Image");
DEFINE_string(name,"img_turntable","Out Image name prefix");
DEFINE_bool(draw,false,"If TRUE only draw not output any Images");
DEFINE_int32(offx,0,"Offset X");
DEFINE_int32(offy,0,"Offset Y");
DEFINE_int32(cr,310,"Center of circle radius");
DEFINE_int32(r,230,"Radius of outer circle");
DEFINE_int32(a,60,"Angle");
DEFINE_int32(n,3,"Unit num");

std::string dir = "save";
static bool OnlyDraw = false;

std::vector<cv::Mat> cut(float angle, int n, float circle_r, float r,glm::vec2 offset, const cv::Mat& mat,bool print = false);

void save_imgs(const std::string& dir, const std::string& name, int idx,std::vector<cv::Mat> imgs);

cv::Mat cut_center(glm::vec2 o, const cv::Mat& mat, float r);

int main(int argc, char** argv)
{
    google::ParseCommandLineFlags(&argc,&argv,true);
    OnlyDraw = FLAGS_draw;
    glm::vec2 offset(FLAGS_offx,FLAGS_offy);
    float angle = static_cast<float>(FLAGS_a);
    float circleR = static_cast<float>(FLAGS_cr) * 0.5f;
    float r = static_cast<float>(FLAGS_r) * 0.5f;
    int n = FLAGS_n; 
    cv::Mat img = cv::imread(FLAGS_img,-1);
    if (img.empty()) return  -1;
    cv::namedWindow("example1", cv::WINDOW_AUTOSIZE);
    int idx = 1;
    int rot = 0;
    int l = 360 / static_cast<int>(angle);
    glm::vec2 o = glm::vec2(img.size[1] / 2, img.size[0] / 2) + offset;
    auto center_img = cut_center(o,img,circleR);
    if(OnlyDraw)
    {
        cv::imshow("example1",center_img);
        cv::waitKey();
    }
    for(int i = 0;i < l;++i)
    {
        cv::Mat dst;
        if(rot != 0)
        {
            auto rot_mat = cv::getRotationMatrix2D(cv::Point2f(o.x,o.y), rot, 1);
            cv::warpAffine(img,dst,rot_mat,cv::Size(img.size[1] , img.size[0]));           
        }
        else dst = img;
        std::vector<cv::Mat> res = cut(angle,n,circleR,r,offset,dst,i == 0);
        if(OnlyDraw)
        {
            cv::imshow("example1",dst);
            cv::waitKey();
        }
        save_imgs(dir,FLAGS_name,idx,res);
        rot -= angle;
        idx += 1;
    }
    if(!OnlyDraw)
    {
        auto path = FLAGS_name += "_0.png";
        cv::imwrite(path.c_str(),center_img );
    }
    cv::destroyWindow("example1");
    return 0;
}

glm::vec2 rotate(glm::vec2 v,float deg)
{
    return glm::mat2(
        glm::cos(glm::radians(deg)),glm::sin(glm::radians(deg)),
        -glm::sin(glm::radians(deg)),glm::cos(glm::radians(deg))
    ) * v;
}

std::tuple<glm::vec2,glm::vec2,glm::vec2> calcInter(glm::vec2 vec, float radius,float angle);

std::tuple<glm::vec2,glm::vec2,glm::vec2,glm::vec2> calcBound(glm::vec2 o, float br, float er, float angle);

std::vector<cv::Mat> cut(float angle, int n, float circle_r, float r,glm::vec2 offset, const cv::Mat& mat,bool print)
{
    std::vector<cv::Mat> res;
    float radius = circle_r;
    glm::vec2 o(mat.size[1] / 2, mat.size[0] / 2);
    o += offset;

    for(int i = 0;i < n;++i)
    {
        auto bottomR = radius + r;
        auto[lt,rt,lb,rb] = calcBound(o,radius,bottomR,angle);
        auto len = (lt.y - o.y) + ((lb.y - lt.y) / 2);
        if(print)printf("offset = %f width = %f height = %f\n",len,ceilf(rt.x - lt.x),ceilf(lb.y - lt.y));
        if(OnlyDraw)
        {
            cv::rectangle(mat,cv::Point(lt.x,lt.y),cv::Point(rb.x,rb.y),CV_RGB(255,0,0));
        }else
        {
            res.push_back(mat(cv::Range(lt.y,rb.y),cv::Range(lt.x,rb.x)));
        }
        radius = bottomR;
    }
    return res;
}

std::tuple<glm::vec2,glm::vec2,glm::vec2> calcInter(glm::vec2 o, float radius,float angle)
{
    glm::vec2 dir  = glm::vec2(0,radius);
    glm::vec2 l =  o + rotate(dir,angle * 0.5f);
    glm::vec2 r = o + rotate(dir,angle * -0.5f);
    return std::make_tuple(l,r,o + dir);
}

std::tuple<glm::vec2, glm::vec2, glm::vec2, glm::vec2> calcBound(glm::vec2 o, float br, float er, float angle)
{
    auto [lt,rt,mt] = calcInter(o,br,angle);
    auto [lb,rb,mb] = calcInter(o,er,angle);
    return std::make_tuple(
        glm::vec2(lb.x,lt.y), //left
        glm::vec2(rb.x,rt.y),
        glm::vec2(lb.x,mb.y),
        glm::vec2(rb.x,mb.y));
}

void save_imgs(const std::string& dir, const std::string& name, int idx,std::vector<cv::Mat> imgs)
{
    for(int i = 0;i < imgs.size();++i)
    {
        char tmp[500] = {0};
        sprintf(tmp,"%s_%d_%d.png",name.data(),idx,i + 1);
        std::string path(tmp);
        cv::imwrite(path.c_str(),imgs[i]);
    }
}

cv::Mat cut_center(glm::vec2 o, const cv::Mat& mat, float r)
{
    auto b = o - glm::vec2(r,r);
    auto e = o + glm::vec2(r,r);
    auto img = mat(cv::Range(b.y,e.y),cv::Range(b.x,e.x)).clone();
    cv::circle(img,cv::Point(r,r),r,cv::Scalar(0, 0, 0, 255),4);
    for(auto x=0;x<img.rows;x++){
        for(auto y=0;y<img.cols;y++){
            int xx = r - x;
            int yy = r - y;
            float radius = hypot((float)xx,(float)yy);
            auto& pixel = img.at<cv::Vec4b>(x,y);
            if(radius > r){ pixel[3] = 50;}
            if(radius > r+2){ pixel[3] = 0;}// partial
        }
    }
    return img;
}
