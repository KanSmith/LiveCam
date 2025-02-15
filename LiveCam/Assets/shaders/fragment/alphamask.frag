#version 450
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec3 Position;

struct Line
{
	vec2 A;
	vec2 B;
	float length;
};

uniform Line edges[24];
uniform int edgesCount;
uniform float edgeFadingDistance;
uniform float newAlpha;

uniform int WinWidth;
uniform int WinHeight;

uniform sampler2D Texture;

uniform sampler2D AlphaMask;

float getVertexDistance(vec2 point, vec2 vertex, float min)
{
	vec2 radius = point - vertex;
	float dist = dot(radius, radius);
	if (dist < edgeFadingDistance * edgeFadingDistance && dist < min * min)
	{
		min = sqrt(dist);
	}
	
	return min;
}

float getEdgeDistance(vec2 point, Line edge, float min)
{
	vec2 direction = edge.B - edge.A;
	
	if (dot(direction, point - edge.A) > 0 &&
		dot(-direction, point - edge.B) > 0)
	{
		// the point is projected on the line segment between its boundary points

		float dist = abs((direction.y * point.x - direction.x * point.y + edge.B.x * edge.A.y - edge.B.y * edge.A.x) / edge.length);
		if (dist < min)
		{
			min = dist;
		}
	}
	else
	{
		min = getVertexDistance(point, edge.A, min);
	}
	
	return min;
}

bool onTheLeftSide(vec2 point, vec2 A, vec2 B)
{
	point -= A;
	vec2 direction = B - A;
	
	return (point.x * direction.y - point.y * direction.x) > 0;
}

float getAlphaScale(vec2 point)
{
	float min = 1e6;
	
	for (int i = 0; i < edgesCount; ++i)
	{
		if (onTheLeftSide(point, edges[i].A, edges[i].B))
		{
			min = getEdgeDistance(point, edges[i], min);
		}
	}
	
	return min < edgeFadingDistance ? min / edgeFadingDistance : 1.0;
}

void main()
{
	vec2 winCoords = vec2(gl_FragCoord.x / WinWidth, gl_FragCoord.y / WinHeight);
	
	float oldAlpha = texture2D(AlphaMask, winCoords).a;
	
	float alpha = oldAlpha + (newAlpha - oldAlpha) * getAlphaScale(Position.xy); 
	
	FragColor = vec4(0, 0, 0, alpha);
    BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}