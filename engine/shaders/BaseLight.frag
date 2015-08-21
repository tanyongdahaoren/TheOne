const char* BaseLight_frag = STRINGIFY(

in vec2 o_tex_coord;
in vec3 o_world_normal;
in vec3 o_world_pos;

out vec4 color;

//基础光照模型基类
struct BaseLight
{                                                                                   
    vec3 color;//光颜色
	float ambientIntensity;//环境光强度
	float diffuseIntensity;//漫反射光强度
};

//方向光
struct DirectionLight
{
	BaseLight base;
	vec3 direction;                                                                 
};

//点光
struct PointLight                                                                           
{                                                                                           
    BaseLight base;                                                                         
    vec3 world_pos;                                                                          
    float constant;                                                                 
    float linear;                                                                   
    float exp;                                                                      
};

//聚光灯
struct SpotLight                                                                            
{                                                                                           
    PointLight base;
    vec3 direction;
    float cutoff;
};

//uniform 方向光
uniform DirectionLight u_direction_light;

//uniform 点光
uniform int u_point_light_num;
uniform PointLight u_point_lights[kMaxPointLightNum];

//uniform 聚光灯
uniform int u_spot_light_num;
uniform SpotLight u_spot_light[kMaxSpotLightNum]; 

uniform vec3 u_world_eyepos;
uniform float u_specular_intensity;
uniform float u_specular_power;

//计算
vec4 CalculateLightInternal(BaseLight light, vec3 direction)                   
{
	//环境光                                                                                    
    vec4 ambientColor = vec4(light.color * light.ambientIntensity, 1.0f);
    
	//漫反射系数
	//***取决于方向光向量与顶点法线向量的夹角
	float diffuseFactor = dot(o_world_normal, -direction);

	vec4 diffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 specularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (diffuseFactor > 0)
	{
		diffuseColor = vec4(light.color * light.diffuseIntensity * diffuseFactor, 1.0f);

		vec3 vertex2eye = normalize(u_world_eyepos - o_world_pos);
        vec3 lightReflect = normalize(reflect(direction, o_world_normal));   
		float specularFactor = dot(vertex2eye, lightReflect);
		if (specularFactor > 0)
		{                                                           
            specularFactor = pow(specularFactor, u_specular_power);
			specularColor = vec4(light.color * u_specular_intensity * specularFactor, 1.0f);
        }                                                  
    }                                                                                       

    return (ambientColor + diffuseColor + specularColor);                                   
}

//计算点光源
vec4 CalculatePointLight(PointLight light)                                                 
{                                                                                           
    vec3 light2pixel = o_world_pos - light.world_pos;                         
    float distance = length(light2pixel);                                                
    light2pixel = normalize(light2pixel);                                             
                                                                                            
    vec4 color = CalculateLightInternal(light.base, light2pixel);       
    float attenuation =  light.constant +                               
                         light.linear * distance +                      
                         light.exp * distance * distance;               
                                                                                            
    return color / attenuation;                                                             
}

//计算聚光灯                                                                                    
vec4 CalculateSpotLight(SpotLight light)                                                
{                                                                                           
    vec3 light2pixel = normalize(o_world_pos - light.base.world_pos);                             
    float spotFactor = dot(light2pixel, light.direction);                                      
                                                                                            
    if (spotFactor > light.cutoff)
	{                                                            
        vec4 color = CalculatePointLight(light.base);                                        
        return color * (1.0 - (1.0 - spotFactor) * 1.0/(1.0 - light.cutoff));                   
    }                                                                                       
    else
	{                                                                                  
        return vec4(0,0,0,0);                                                               
    }                                                                                       
}

void main()
{
	vec4 totalLight;
	
	vec4 directionLight = CalculateLightInternal(u_direction_light.base, u_direction_light.direction);                                                                   
	
	totalLight += directionLight;

	for (int i = 0 ; i < u_point_light_num ; i++)
	{                                           
        totalLight += CalculatePointLight(u_point_lights[i]);                                            
    }
    
	for (int i = 0 ; i < u_spot_light_num ; i++)
	{                                           
        totalLight += CalculateSpotLight(u_spot_light[i]);                                            
    }

	color = texture2D(u_sampler, o_tex_coord.xy) * totalLight;
}
);
