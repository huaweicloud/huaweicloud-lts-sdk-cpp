## 使用前提
- 要使用LTS C++ SDK ，您需要拥有云账号以及该账号对应的 Access Key（AK）和 Secret Access Key（SK）。 请在华为云控制台“我的凭证-访问密钥”页面上创建和查看您的 AK&SK 。更多信息请查看 [访问密钥](https://support.huaweicloud.com/usermanual-ca/zh-cn_topic_0046606340.html)  。
- 要使用LTS C++ SDK 上报日志 ，您需要确认已在 [华为云控制台](https://console.huaweicloud.com/console/?locale=zh-cn&region=cn-north-4#/home) 开通LTS服务
- LTS C++ SDK 适用于
    - C++ 11 及以上版本
    - Linux系统


使用LTS C++ SDK时，有以下配置参数：

| 参数名称               | 描述                      | 类型     | 是否必填           | 默认值 |
|--------------------|-------------------------|--------|----------------|-----|
| $projectId          | 华为云帐号的项目ID（project id）。 | String | 必填             |     |
| $ak        | 华为云帐号的AK。               | String | 必填             |     |
| $sk    | 华为云帐号的SK。               | String | 必填             |     |
| $region         | 云日志服务的区域。               | String | 必填             |     |
| $endpoint           | 日志上报的目的地址。              | String | 必填             |     |
| $logGroup        | 上报日志的日志组。               | String | 必填             |     |
| $logStream       | 上报日志的日志流。               | String | 必填             |     |
| $httpTimeOut       | 调用LTS接口上报日志的超时时间        | int    | 必填(建议60s)      |     |
| $batchSizeThreshold       | 一个批次最大的日志量。             | int | 必填（建议512*1024） |     |
| $lingerMs       | 日志在batch中停留最长时间。        | int | 必填（建议2000）     |     |
## 三方库
| 三方库                          | License信息        | 链接                                       | 版本    |
|---------------------------------|--------------------|--------------------------------------------|---------|
| openssl | Apache-2.0 license | https://github.com/openssl/openssl       | OpenSSL_1_1_1g   |
| zlib                       |                    | https://github.com/madler/zlib     | v1.2.11 |
| curl                 |                    | https://github.com/curl/curl | curl-7_78_0  |
| jsoncpp                 | MIT license        | https://github.com/open-source-parsers/jsoncpp | 0.10.7  |

## 代码示例
```c++
#include <sdk/Config.h>
#include <sdk/Producer.h>
#include <client/logger.h>

Config* GetConfig() {
    Config* cfg = new Config();

    cfg->Endpoint = "endpoint";
    cfg->AccessKeyID = "ak";
    cfg->AccessKeySecret = "sk";
    cfg->RegionId = "region";
    cfg->ProjectId = "pid";

    cfg->validateProducerConfig();

    return cfg;
}

Log* generateLog() {
    std::vector<std::string> logContentArr = {"content for this test no.1"};
    std::map<std::string, std::string> labels = {{"keyA","valueA"}};

    return GenerateLog(logContentArr, labels);
}

void userCallBack(Result* result) {
    if (result == nullptr) {
        return;
    }

    const char* isSuccess = "false";
    if (result->successful) {
        isSuccess = "true";
    }

    LogPrint(LOG_LEVEL_WARN, "userCallBack [%s]", isSuccess);
}

int main() {
    Config *cfg = GetConfig();

    Producer *producerInstance = InitProducer(cfg);
    producerInstance->Start();

    CallBack *cb = new CallBack();
    cb->Success = userCallBack;
    cb->Fail = userCallBack;

    Log *log = generateLog();
    producerInstance->SendLog("groupId", "streamId", std::vector<Log *>{log}, cb);

    SleepMs(60 * 60 * 1000);

    producerInstance->Close();

    delete cb;

    delete cfg;
}
```