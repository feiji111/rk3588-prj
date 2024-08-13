#ifndef RKNNPOOL_H
#define RKNNPOOL_H

#include "ThreadPool.hpp"
#include <vector>
#include <iostream>
#include <mutex>
#include <queue>
#include <memory>

template <typename rknnModel, typename inputType, typename outputType>
class rknnPool
{
private:
    int threadNum;
    std::string modelPath;

    long long id;
    std::mutex idMtx, queueMtx;
    std::unique_ptr<dpool::ThreadPool> pool;
    std::queue<std::future<outputType>> futs;
    std::vector<std::shared_ptr<rknnModel>> models;

protected:
    int getModelId() {
        std::lock_guard<std::mutex> lock(idMtx);
        int modelId = id % threadNum;
        id++;
        return modelId;   
    }

public:
    rknnPool(const std::string modelPath, int threadNum) {
        this->modelPath = modelPath;
        this->threadNum = threadNum;
        this->id = 0;
    };
    int init() {
        try
        {
            this->pool = std::make_unique<dpool::ThreadPool>(this->threadNum);
            for (int i = 0; i < this->threadNum; i++)
                models.push_back(std::make_shared<rknnModel>(this->modelPath.c_str()));
        }
        catch (const std::bad_alloc &e)
        {
            std::cout << "Out of memory: " << e.what() << std::endl;
            return -1;
        }
        // Initialize the model
        for (int i = 0, ret = 0; i < threadNum; i++)
        {
            ret = models[i]->init(models[0]->get_pctx(), i != 0);
            if (ret != 0)
                return ret;
        }
        return 0;
    };
    // Model inference
    int put(inputType inputData) {
        std::lock_guard<std::mutex> lock(queueMtx);
        futs.push(pool->submit(&rknnModel::infer, models[this->getModelId()], inputData));
        return 0;
    }
    // Get the results of your inference
    int get(outputType &outputData) {
        std::lock_guard<std::mutex> lock(queueMtx);
        if(futs.empty() == true)
            return 1;
        outputData = futs.front().get();
        futs.pop();
        return 0;        
    }
    ~rknnPool() {
        while (!futs.empty())
        {
            outputType temp = futs.front().get();
            futs.pop();
        }
    }
};

#endif
