#version 330
// To Framebuffer
out vec4 FragColor;

in VS_OUT {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 ShadowCoord;
    mat3 TBN;
} fs_in;

uniform vec3 gLightPos;
uniform vec3 gViewPos;

uniform sampler2D AmbientMap;
uniform sampler2D DiffuseMap;
uniform sampler2D SpecularMap;
uniform sampler2D BumpMap;
uniform sampler2D ReflectionMap;

uniform sampler2D ShadowMap;
uniform samplerCube CubeMap;

uniform float shadowExpC;

float near_plane = 0.1;
float far_plane = 1000;

float shadowBias = 0.005;
float shadowBiasClamp = 0.01;

float reflection_threshold = 0.2;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

float ShadowCalculation(vec4 shadowCoord, float bias) {
	float visible = 1.0;
	vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
	float lightDepth = texture(ShadowMap, projCoords.xy).r;
	if (lightDepth < projCoords.z-bias &&
		projCoords.z <= 1.0) {
		visible = 0;
		/*
		if (lightDepth < projCoords.z-bias) {
			visible = 0;
		} else {
			visible = lightDepth / exp(shadowExpC*projCoords.z);
		}*/
	}
	return visible;
}

void main () {
    vec4 ambientColor = texture(AmbientMap, fs_in.TexCoord);
    vec4 diffuseColor = texture(DiffuseMap, fs_in.TexCoord);
    vec4 specularColor = texture(SpecularMap, fs_in.TexCoord);
    float reflection = texture(ReflectionMap, fs_in.TexCoord).r;

    vec3 normal = texture(BumpMap, fs_in.TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal); 
    //vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(gViewPos - fs_in.WorldPos);
    vec3 cubeMapReflect = reflect(viewDir, normal);
    vec3 envMap = texture(CubeMap, cubeMapReflect).rgb;
    
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = lightColor * ambientColor.rgb;
    // Diffuse
    vec3 lightDir = normalize(gLightPos - fs_in.WorldPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lightColor * (diff * diffuseColor.rgb);
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), shadowExpC);
    vec3 specular = lightColor * (spec * specularColor.rgb);
    // Env
    vec3 env = diff * envMap;

    // Calculate shadow
    float bias = shadowBias*tan(acos(dot(normal, lightDir))); // cosTheta is dot( n,l ), clamped between 0 and 1
	bias = clamp(bias, 0, shadowBiasClamp);
    //float visible = ShadowCalculation(fs_in.ShadowCoord, bias);       
    float visible = 1.0;
    vec3 lighting = ambient + visible * (diffuse + mix(specular, env, reflection));

    FragColor = vec4(lighting, 1.0f);
    
    // Used to debug shadow buffer
	//float depth = texture(Texture0, TexCoordFS).r;
    //frag_color = vec4(vec3(LinearizeDepth(depth) / far_plane), 1.0); // perspective
}