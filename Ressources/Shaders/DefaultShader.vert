#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 worldProjection;
    mat4 viewMatrix;
	vec3 cameraLocation;
	float time;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;

layout(push_constant) uniform PushConsts {
	mat4 model;
} primitive;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragTangent;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
	gl_Position = ubo.worldProjection * ubo.viewMatrix * primitive.model * vec4(inPosition.xyz, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
	fragPos = inPosition;
	fragNormal = inNormal;
	fragTangent = inTangent;
}