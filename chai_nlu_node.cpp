/*************************************************************************
	> File Name: chai_nlu_node.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年06月09日 星期五 23时14分54秒
 ************************************************************************/
#include<ros/ros.h>
#include<std_msgs/String.h>
#include<iostream>
#include<sstream>
#include<jsoncpp/json/json.h>
#include<curl/curl.h>
#include<string>
#include<exception>

using namespace std;

int flag = 0;
string result;

 int writer(char *data, size_t size, size_t nmemb, string *writerData)
 {
         unsigned long sizes = size * nmemb;
         if (writerData == NULL)
             return -1;
      
         writerData->append(data, sizes);
      
         return sizes;

 }


 int parseJsonResonse(string input)
 {
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(input, root);
        if(!parsingSuccessful)
     {
                std::cout<<"!!! Failed to parse the response data"<< std::endl;
                 return -1;
            
     }
        const Json::Value code = root["code"];
        const Json::Value text = root["text"];
        result = text.asString();
        flag = 1;
      
        std::cout<< "response code:" << code << std::endl;
        std::cout<< "response text:" << result << std::endl;
      
        return 0;

 }

 int HttpPostRequest(string input)
 {
         string buffer;
      
         std::string strJson = "{";
         strJson += "\"key\" : \"58b7f75ebbd24148b355561c2545f734\","; //双引号前加／防转仪
         strJson += "\"info\" : ";
         strJson += "\"";
         strJson += input;
         strJson += "\"";
         strJson += "}";
      
         std::cout<<"post json string: " << strJson << std::endl;
          try
     {
         CURL *pCurl = NULL;
         CURLcode res;
         curl_global_init(CURL_GLOBAL_ALL);
         pCurl = curl_easy_init();
         if (NULL != pCurl)
         {
             curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10);
             curl_easy_setopt(pCurl, CURLOPT_URL, "http://www.tuling123.com/openapi/api");
             curl_slist *plist = curl_slist_append(NULL,"Content-Type:application/json;charset=UTF-8");
             curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, plist);
            curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strJson.c_str());
             curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, writer);
             curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &buffer);
             res = curl_easy_perform(pCurl);
             if (res != CURLE_OK)
             { printf("curl_easy_perform() failed:%s\n",curl_easy_strerror(res)); }
             curl_easy_cleanup(pCurl);
}
         curl_global_cleanup();
}
     catch (std::exception &ex)
     {printf("curl exception %s.\n", ex.what());
     }

     if(buffer.empty())
     {std::cout<< "!!! ERROR The Tuling sever response NULL" << std::endl;}
     else
     {parseJsonResonse(buffer);}
     return 0;
 }

void arvCallBack(const std_msgs::String::ConstPtr &msg)
{
        std::cout<<"your quesion is: " << msg->data << std::endl;
        HttpPostRequest(msg->data);

}

int main(int argc, char **argv)
{
        ros::init(argc, argv,"chai_nlu_node");
        ros::NodeHandle nd;
     
        ros::Subscriber sub = nd.subscribe("voice/chai_nlu_topic", 10, arvCallBack);
        ros::Publisher pub = nd.advertise<std_msgs::String>("/voice/chai_tts_topic", 10);
        ros::Rate loop_rate(10);
     
        while(ros::ok())
    {
                if(flag)
        {
                        std_msgs::String msg;
                        msg.data = result;
                        pub.publish(msg);
                        flag = 0;
                    
        }
                ros::spinOnce();
                loop_rate.sleep();
            
    }
     
     

}
