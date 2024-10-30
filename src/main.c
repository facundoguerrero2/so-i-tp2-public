#include <stdio.h>
#include <stdlib.h> 
#include <cjson/cJSON.h>


int main(void){

    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root,"name","Facu");
    cJSON_AddNumberToObject(root,"age",22);
    cJSON_AddBoolToObject(root,"is_student",1);

    cJSON* hobbies = cJSON_CreateArray();

    cJSON_AddItemToArray(hobbies, cJSON_CreateString("play Guitar"));
    cJSON_AddItemToObject(root,"hobbies",hobbies);

    char* json_string = cJSON_Print(root);
    printf("%s\n", json_string);

    cJSON_Delete(root);
    free(json_string);

    return 0;


}