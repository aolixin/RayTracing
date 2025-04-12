#version 460 core

#define PI              3.1415926
#define INF             114514.0
#define SIZE_TRIANGLE   7
#define SIZE_BVHNODE    4
#define SIZE_MATERIAL   6
#define INF             114514.0
#define MAX_DEPTH       10
#define MAX_SSP         1


out vec4 FragColor;

in vec3 pix;
in vec2 TexCoords;

uniform int frameCounter;
uniform int width;
uniform int height;
uniform vec3 cameraPos;
uniform mat4 cameraRotate;

uniform samplerBuffer triangles;
uniform int nTriangles;

uniform samplerBuffer nodes;
uniform samplerBuffer materials;

uniform samplerCube environmentMap;
uniform sampler2D lastFrame;
uniform sampler2D hdrMap;
uniform sampler2D hdrCache;
uniform int hdrResolution;


uint seed = uint(
    uint((pix.x * 0.5 + 0.5) * width) * uint(1973) +
    uint((pix.y * 0.5 + 0.5) * height) * uint(9277) +
    uint(frameCounter) * uint(26699)) | uint(1);

uint wang_hash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

float rand() {
    return float(wang_hash(seed)) / 4294967296.0;
}

vec3 SampleHemisphere(float xi_1, float xi_2) {
    float z = xi_1;
    float r = max(0.0, sqrt(1.0 - z * z));
    float phi = 2.0 * PI * xi_2;
    return vec3(r * cos(phi), r * sin(phi), z);
}



vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv /= vec2(2.0 * PI, PI);
    uv += 0.5;
    uv.y = 1.0 - uv.y;
    return uv;
}

vec3 SampleHdr(float xi_1, float xi_2) {
    vec2 xy = texture2D(hdrCache, vec2(xi_1, xi_2)).rg; // x, y
    xy.y = 1.0 - xy.y; // flip y

    // 获取角度
    float phi = 2.0 * PI * (xy.x - 0.5);    // [-pi ~ pi]
    float theta = PI * (xy.y - 0.5);        // [-pi/2 ~ pi/2]   

    // 球坐标计算方向
    vec3 L = vec3(cos(theta) * cos(phi), sin(theta), cos(theta) * sin(phi));

    return L;
}



vec3 SampleEnvCubeMap(vec3 v) {
    vec3 envColor = texture(environmentMap, v).rgb;
    return envColor;
}

vec3 toNormalHemisphere(vec3 v, vec3 N) {
    vec3 helper = vec3(1, 0, 0);
    if (abs(N.x) > 0.999) helper = vec3(0, 0, 1);
    vec3 tangent = normalize(cross(N, helper));
    vec3 bitangent = normalize(cross(N, tangent));
    return v.x * tangent + v.y * bitangent + v.z * N;
}

vec3 SampleCosineHemisphere(float xi_1, float xi_2, vec3 N) {
    float r = sqrt(xi_1);
    float theta = xi_2 * 2.0 * PI;
    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(1.0 - x * x - y * y);

    vec3 L = toNormalHemisphere(vec3(x, y, z), N);
    return L;
}

void GetTangent(vec3 N, inout vec3 tangent, inout vec3 bitangent) {
    vec3 helper = vec3(1, 0, 0);
    if (abs(N.x) > 0.999) helper = vec3(0, 0, 1);
    bitangent = normalize(cross(N, helper));
    tangent = normalize(cross(N, bitangent));
}


const uint V[8 * 32] = {
2147483648u, 1073741824u, 536870912u, 268435456u, 134217728u, 67108864u, 33554432u, 16777216u, 8388608u, 4194304u, 2097152u, 1048576u, 524288u, 262144u, 131072u, 65536u, 32768u, 16384u, 8192u, 4096u, 2048u, 1024u, 512u, 256u, 128u, 64u, 32u, 16u, 8u, 4u, 2u, 1u, 2147483648u, 3221225472u, 2684354560u, 4026531840u, 2281701376u, 3422552064u, 2852126720u, 4278190080u, 2155872256u, 3233808384u, 2694840320u, 4042260480u, 2290614272u, 3435921408u, 2863267840u, 4294901760u, 2147516416u, 3221274624u, 2684395520u, 4026593280u, 2281736192u, 3422604288u, 2852170240u, 4278255360u, 2155905152u, 3233857728u, 2694881440u, 4042322160u, 2290649224u, 3435973836u, 2863311530u, 4294967295u, 2147483648u, 3221225472u, 1610612736u, 2415919104u, 3892314112u, 1543503872u, 2382364672u, 3305111552u, 1753219072u, 2629828608u, 3999268864u, 1435500544u, 2154299392u, 3231449088u, 1626210304u, 2421489664u, 3900735488u, 1556135936u, 2388680704u, 3314585600u, 1751705600u, 2627492864u, 4008611328u, 1431684352u, 2147543168u, 3221249216u, 1610649184u, 2415969680u, 3892340840u, 1543543964u, 2382425838u, 3305133397u, 2147483648u, 3221225472u, 536870912u, 1342177280u, 4160749568u, 1946157056u, 2717908992u, 2466250752u, 3632267264u, 624951296u, 1507852288u, 3872391168u, 2013790208u, 3020685312u, 2181169152u, 3271884800u, 546275328u, 1363623936u, 4226424832u, 1977167872u, 2693105664u, 2437829632u, 3689389568u, 635137280u, 1484783744u, 3846176960u, 2044723232u, 3067084880u, 2148008184u, 3222012020u, 537002146u, 1342505107u, 2147483648u, 1073741824u, 536870912u, 2952790016u, 4160749568u, 3690987520u, 2046820352u, 2634022912u, 1518338048u, 801112064u, 2707423232u, 4038066176u, 3666345984u, 1875116032u, 2170683392u, 1085997056u, 579305472u, 3016343552u, 4217741312u, 3719483392u, 2013407232u, 2617981952u, 1510979072u, 755882752u, 2726789248u, 4090085440u, 3680870432u, 1840435376u, 2147625208u, 1074478300u, 537900666u, 2953698205u, 2147483648u, 1073741824u, 1610612736u, 805306368u, 2818572288u, 335544320u, 2113929216u, 3472883712u, 2290089984u, 3829399552u, 3059744768u, 1127219200u, 3089629184u, 4199809024u, 3567124480u, 1891565568u, 394297344u, 3988799488u, 920674304u, 4193267712u, 2950604800u, 3977188352u, 3250028032u, 129093376u, 2231568512u, 2963678272u, 4281226848u, 432124720u, 803643432u, 1633613396u, 2672665246u, 3170194367u, 2147483648u, 3221225472u, 2684354560u, 3489660928u, 1476395008u, 2483027968u, 1040187392u, 3808428032u, 3196059648u, 599785472u, 505413632u, 4077912064u, 1182269440u, 1736704000u, 2017853440u, 2221342720u, 3329785856u, 2810494976u, 3628507136u, 1416089600u, 2658719744u, 864310272u, 3863387648u, 3076993792u, 553150080u, 272922560u, 4167467040u, 1148698640u, 1719673080u, 2009075780u, 2149644390u, 3222291575u, 2147483648u, 1073741824u, 2684354560u, 1342177280u, 2281701376u, 1946157056u, 436207616u, 2566914048u, 2625634304u, 3208642560u, 2720006144u, 2098200576u, 111673344u, 2354315264u, 3464626176u, 4027383808u, 2886631424u, 3770826752u, 1691164672u, 3357462528u, 1993345024u, 3752330240u, 873073152u, 2870150400u, 1700563072u, 87021376u, 1097028000u, 1222351248u, 1560027592u, 2977959924u, 23268898u, 437609937u
};

// 格林码 
uint grayCode(uint i) {
    return i ^ (i >> 1);
}

// 生成第 d 维度的第 i 个 sobol 数
float sobol(uint d, uint i) {
    uint result = 0;
    uint offset = d * 32;
    for (uint j = 0; i != 0; i >>= 1, j++)
    if ((i & 1) != 0)
    result ^= V[j + offset];

    return float(result) * (1.0f / float(0xFFFFFFFFU));
}

// 生成第 i 帧的第 b 次反弹需要的二维随机向量
vec2 sobolVec2(uint i, uint b) {
    float u = sobol(b * 2, grayCode(i));
    float v = sobol(b * 2 + 1, grayCode(i));
    return vec2(u, v);
}

vec2 CranleyPattersonRotation(vec2 p) {
    uint pseed = uint(
        uint((pix.x * 0.5 + 0.5) * width) * uint(1973) +
        uint((pix.y * 0.5 + 0.5) * height) * uint(9277) +
        uint(114514 / 1919) * uint(26699)) | uint(1);

    float u = float(wang_hash(pseed)) / 4294967296.0;
    float v = float(wang_hash(pseed)) / 4294967296.0;

    p.x += u;
    if (p.x > 1) p.x -= 1;
    if (p.x < 0) p.x += 1;

    p.y += v;
    if (p.y > 1) p.y -= 1;
    if (p.y < 0) p.y += 1;

    return p;
}

float sqr(float x) {
    return x * x;
}

float SchlickFresnel(float u) {
    float m = clamp(1 - u, 0, 1);
    float m2 = m * m;
    return m2 * m2 * m; // pow(m,5)
}



uniform sampler2D screenTexture;

void main()
{
    if (pix.y > 0) {
        FragColor = texture2D(hdrMap, vec2(pix.x * 0.5 + 0.5, 1 - pix.y));
    } else {
        FragColor = texture2D(hdrCache, vec2(pix.x * 0.5 + 0.5, -pix.y));
    }

    // 输出亮度

    float pdf = texture2D(hdrCache, vec2(pix.x * 0.5 + 0.5, -pix.y * 0.5 + 0.5)).b;
    FragColor = vec4(pdf) * 100000;

    FragColor = texture2D(hdrMap, vec2(pix.x * 0.5 + 0.5, -pix.y * 0.5 + 0.5));

    // 采样点
    for (int i = 0; i < 1000; i++) {
        vec2 uv = sobolVec2(i + 1, 0); // 取 1,2 维度的 sobol 数做随机数
        vec2 texcoord = texture2D(hdrCache, uv).rg;
        texcoord.y = 1.0 - texcoord.y;
        if (distance(pix.xy * 0.5 + 0.5, texcoord) < 0.005) FragColor.rgb = vec3(1, 0, 0);
    }

} 