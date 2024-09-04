#include "command.h"

Command::Command(): url(""),
    use_hardware(0), 
    width(640), 
    height(480), 
    preset("ultrafast"), 
    tune("zerolatency"), 
    profile("high"), 
    fps(30), 
    capture_name("x11grab"), 
    protocol("rtsp"), 
    trans_protocol("udp"),
    mode("detection") {}

// Getter method implementations
std::string Command::getUrl() const {
    return url;
}

int Command::getUseHardware() const {
    return use_hardware;
}

std::string Command::getProtocol() const {
    return protocol;
}

int Command::getWidth() const {
    return width;
}

int Command::getHeight() const {
    return height;
}

std::string Command::getPreset() const {
    return preset;
}

std::string Command::getTune() const {
    return tune;
}

std::string Command::getProfile() const {
    return profile;
}

int Command::getFps() const {
    return fps;
}

std::string Command::getCaptureName() const {
    return capture_name;
}

std::string Command::getTransProtocol() const {
    return trans_protocol;
}

std::string Command::getCodecName() const {
    return trans_protocol;
}

std::string Command::getSrc() const {
    return src;
}

std::string Command::getDetectionModel() const {
    return detection_model;
}

std::string Command::getTrackModelHead() const {
    return track_model_head;
}

std::string Command::getTrackModelBackbone() const {
    return track_model_backbone;
}

std::string Command::getMode() const {
    return mode;
}

std::string Command::getBinaryModel() const {
    return binary_model;
}

// Setter method implementations
void Command::setUrl(const std::string& url) {
    this->url = url;
}

void Command::setUseHardware(int use_hardware) {
    this->use_hardware = use_hardware;
}

void Command::setProtocol(const std::string& protocol) {
    this->protocol = protocol;
}

void Command::setWidth(int width) {
    this->width = width;
}

void Command::setHeight(int height) {
    this->height = height;
}

void Command::setPreset(const std::string& preset) {
    this->preset = preset;
}

void Command::setTune(const std::string& tune) {
    this->tune = tune;
}

void Command::setProfile(const std::string& profile) {
    this->profile = profile;
}

void Command::setFps(int fps) {
    this->fps = fps;
}

void Command::setCaptureName(const std::string& capture_name) {
    this->capture_name = capture_name;
}

void Command::setTransProtocol(const std::string& trans_protocol) {
    this->trans_protocol = trans_protocol;
}

void Command::setCodecName(const std::string &codec_name) {
    this->codec_name = codec_name;
}

void Command::setSrc(const std::string &src) {
    this->src = src;
}

void Command::setDetectionModel(const std::string &detection_model) {
    this->detection_model = detection_model;
}

void Command::setTrackModelHead(const std::string &track_model_backbone) {
    this->track_model_backbone = track_model_backbone;
}

void Command::setTrackModelBackbone(const std::string &track_model_head) {
    this->track_model_head = track_model_head;
}

void Command::setMode(const std::string &mode) {
    this->mode = mode;
}

void Command::setBinaryModel(const std::string &model_path) {
    this->binary_model = model_path;
}

Command process_command(int argc,char * argv[]) {
    Command res = Command();
    for(int i=1;i<argc;i++){
        std::string flag = argv[i];
        if(flag == "-n" && i+1 < argc){
            res.setUrl(argv[i+1]);
        }
        if(flag == "-nd"){
            //默认网路流
            res.setUrl("rtsp://192.168.1.222:554/media");
        }
        if(flag == "-hd"){
            res.setUseHardware(1);
        }
        if(flag == "-cn" && i+1 < argc){
            res.setCodecName(argv[i+1]);
        }
        if(flag == "-fps" && i+1 < argc){
            res.setFps(atoi(argv[i + 1]));
        }
        if(flag == "-preset" && i+1 < argc){
            res.setPreset(argv[i+1]);
        }
        if(flag == "-profile" && i+1 < argc){
            res.setProfile(argv[i+1]);
        }
        if(flag == "-tune" && i+1 < argc){
            res.setTune(argv[i+1]);
        }
        if(flag == "rtsp" || flag == "rtmp"){
            res.setProtocol(argv[i]);
        }
        if(flag == "tcp" || flag == "udp"){
            res.setTransProtocol(argv[i]);
        }
        if(flag == "-height" && i + 1 < argc) {
            res.setHeight(atoi(argv[i + 1]));
        }
        if(flag == "-width" && i + 1 < argc) {
            res.setWidth(atoi(argv[i + 1]));
        }
        if(flag == "-input" && i + 1 < argc) {
            res.setSrc(argv[i + 1]);
        }
        if(flag == "-detection" && i + 1 < argc) {
            res.setDetectionModel(argv[i + 1]);
        }
        if(flag == "-track_head" && i + 1 < argc) {
            res.setTrackModelHead(argv[i + 1]);
        }
        if(flag == "-track_backbone" && i + 1 < argc) {
            res.setTrackModelBackbone(argv[i + 1]);
        }
        if(flag == "-binary_model" && i + 1 < argc) {
            res.setBinaryModel(argv[i + 1]);
        }
        if(flag == "-mode" && i + 1 < argc) {
            res.setMode(argv[i + 1]);
        }
    }
    return res;
}