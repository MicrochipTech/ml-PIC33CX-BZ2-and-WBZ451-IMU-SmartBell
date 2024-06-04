#ifndef __MODEL_JSON_H__
#define __MODEL_JSON_H__

const char recognition_model_string_json[] = {"{\"NumModels\":1,\"ModelIndexes\":{\"0\":\"BASE_RANK_0\"},\"ModelDescriptions\":[{\"Name\":\"BASE_RANK_0\",\"ClassMaps\":{\"1\":\"Biceps\",\"2\":\"Lateral\",\"3\":\"Shoulder\",\"0\":\"Unknown\"},\"ModelType\":\"PME\",\"FeatureFunctions\":[\"Skewness\",\"Variance\",\"Variance\",\"Minimum\",\"75thPercentile\",\"PositiveZeroCrossings\",\"Maximum\",\"Maximum\"]}]}"};

int32_t recognition_model_string_json_len = sizeof(recognition_model_string_json);

#endif /* __MODEL_JSON_H__ */
