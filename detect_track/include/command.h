#ifndef RK3588_DETECT_TRACK_COMMAND_H_
#define RK3588_DETECT_TRACK_COMMAND_H_

#include <string>

class Command {
private:
    std::string url;
    int use_hardware;
    std::string codec_name;
    std::string src;
    
    int width;
    int height;

    std::string preset; 
    std::string tune; 
    std::string profile; 

    int fps; // 指定fps
    std::string capture_name;// 捕获名字
    std::string protocol;//RTSP or RTMP
    std::string trans_protocol; // 传输协议 TCP/UDP

    //DNN model
    std::string mode;
    std::string detection_model;
    std::string track_model_head;
    std::string track_model_backbone;
    std::string binary_model;

public:
    Command();

    // Getter methods
    std::string getUrl() const;
    int getUseHardware() const;
    std::string getProtocol() const;
    int getWidth() const;
    int getHeight() const;
    std::string getPreset() const;
    std::string getTune() const;
    std::string getProfile() const;
    int getFps() const;
    std::string getCaptureName() const;
    std::string getTransProtocol() const;
    std::string getCodecName() const;
    std::string getSrc() const;
    std::string getDetectionModel() const;
    std::string getTrackModelHead() const;
    std::string getTrackModelBackbone() const;
    std::string getMode() const;
    std::string getBinaryModel() const;

    // Setter methods
    void setUrl(const std::string& url);
    void setUseHardware(int use_hardware);
    void setProtocol(const std::string& protocol);
    void setWidth(int width);
    void setHeight(int height);
    void setPreset(const std::string& preset);
    void setTune(const std::string& tune);
    void setProfile(const std::string& profile);
    void setFps(int fps);
    void setCaptureName(const std::string& capture_name);
    void setTransProtocol(const std::string& trans_protocol);
    void setCodecName(const std::string& codec_name);
    void setSrc(const std::string& src);
    void setDetectionModel(const std::string& model_path);
    void setTrackModelHead(const std::string& model_path);
    void setTrackModelBackbone(const std::string& model_path);
    void setMode(const std::string& model_path);
    void setBinaryModel(const std::string& model_path);
};

Command process_command(int argc,char * argv[]);

#endif