// shader to decode the pixel data
// the only line that is different between big and little endian is the line which gets which byte to seperate
// there is a "24 - " added in the little endian one

const char* pixelDecoderShaderBigEndian = R"glsl(
#version 330

const vec3 palette[16] = vec3[] (
    vec3(255.0/255.0, 255.0/255.0, 255.0/255.0), vec3(0.0/255.0, 0.0/255.0, 0.0/255.0),
    vec3(230.0/255.0, 41.0/255.0, 55.0/255.0), vec3(0.0/255.0, 228.0/255.0, 48.0/255.0),
    vec3(0.0/255.0, 121.0/255.0, 241.0/255.0), vec3(253.0/255.0, 249.0/255.0, 0.0/255.0),
    vec3(255.0/255.0, 161.0/255.0, 0.0/255.0), vec3(200.0/255.0, 122.0/255.0, 255.0/255.0),
    vec3(130.0/255.0, 130.0/255.0, 130.0/255.0), vec3(80.0/255.0, 80.0/255.0, 80.0/255.0),
    vec3(190.0/255.0, 33.0/255.0, 55.0/255.0), vec3(0.0/255.0, 117.0/255.0, 44.0/255.0),
    vec3(0.0/255.0, 82.0/255.0, 172.0/255.0), vec3(255.0/255.0, 203.0/255.0, 0.0/255.0),
    vec3(127.0/255.0, 106.0/255.0, 79.0/255.0), vec3(255.0/255.0, 109.0/255.0, 194.0/255.0)
);

const float pixelScale = 1.0/12.0;

// this uniform is an int instead of a uint because for some reason i cant copy data into a uint, the uniform
// shader type is just not there
uniform int pixelData[384]; // 1536 bytes, 1536/4 uints (384), 2 pixels per byte (8 per uint), 64*48 resolution

out vec4 fragColor;

// so the data is stored as 4 bits per pixel, or 2 pixels per byte
// the bits correlate to a certain color defined in the pallete (4 bits count to 16, so there are 16 colors in the pallete)
// because glsl doesnt support bytes, i am using uints.
// this shader doesnt account for the fact that endianess can be system specific, i need to change that.

void main() {
    // get position, (0,0) being top left, (64,48) being bottom right
    ivec2 uv = ivec2(gl_FragCoord.x * pixelScale, ((600-24) - gl_FragCoord.y) * pixelScale);

    // get which uint to seperate, 8 pixels per uint
    int index = uv.x + uv.y*64; // index is which pixel to grab, not array index or byte index.
    uint value = (uint(pixelData[index / 8]));

    // ascending data looks like this right now:
    // 0x67452301, notice the byte seperation because this endianess makes no sense?

    // get which byte of the uint to seperate
    // it moves the byte we need to the right side and masks it out
    // (index / 2): every byte has 2 pixels, so every second index needs the same byte
    // ((index / 2) % 4): amount of bytes to move to the right
    // (((index / 2) % 4) * 8): amount of bits to move to the right
    //  & 0xFFu: only include the right most byte
    value = (value >> (((index / 2) % 4) * 8)) & 0xFFu;

    // get which half byte to seperate
    // the data in hex looks like 0x01, the first index needs to be pushed to the right by 1 half byte,
    // and the second shouldn't be pushed
    // (1 - index % 2): amount of half bytes to shift
    // ((1 - index % 2) * 4): amount of bits to shift
    // & 0xFu: only include the right most half byte
    value = (value >> ((1 - index % 2) * 4)) & 0xFu;

    // get the palette color for this value
    fragColor = vec4(palette[value], 1.0);
}
)glsl";

const char* pixelDecoderShaderLittleEndian = R"glsl(
#version 330

const vec3 palette[16] = vec3[] (
    vec3(255.0/255.0, 255.0/255.0, 255.0/255.0), vec3(0.0/255.0, 0.0/255.0, 0.0/255.0),
    vec3(230.0/255.0, 41.0/255.0, 55.0/255.0), vec3(0.0/255.0, 228.0/255.0, 48.0/255.0),
    vec3(0.0/255.0, 121.0/255.0, 241.0/255.0), vec3(253.0/255.0, 249.0/255.0, 0.0/255.0),
    vec3(255.0/255.0, 161.0/255.0, 0.0/255.0), vec3(200.0/255.0, 122.0/255.0, 255.0/255.0),
    vec3(130.0/255.0, 130.0/255.0, 130.0/255.0), vec3(80.0/255.0, 80.0/255.0, 80.0/255.0),
    vec3(190.0/255.0, 33.0/255.0, 55.0/255.0), vec3(0.0/255.0, 117.0/255.0, 44.0/255.0),
    vec3(0.0/255.0, 82.0/255.0, 172.0/255.0), vec3(255.0/255.0, 203.0/255.0, 0.0/255.0),
    vec3(127.0/255.0, 106.0/255.0, 79.0/255.0), vec3(255.0/255.0, 109.0/255.0, 194.0/255.0)
);

const float pixelScale = 1.0/12.0;

// this uniform is an int instead of a uint because for some reason i cant copy data into a uint, the uniform
// shader type is just not there
uniform int pixelData[384]; // 1536 bytes, 1536/4 uints (384), 2 pixels per byte (8 per uint), 64*48 resolution

out vec4 fragColor;

// so the data is stored as 4 bits per pixel, or 2 pixels per byte
// the bits correlate to a certain color defined in the pallete (4 bits count to 16, so there are 16 colors in the pallete)
// because glsl doesnt support bytes, i am using uints.
// this shader doesnt account for the fact that endianess can be system specific, i need to change that.

void main() {
    // get position, (0,0) being top left, (64,48) being bottom right
    ivec2 uv = ivec2(gl_FragCoord.x * pixelScale, ((600-24) - gl_FragCoord.y) * pixelScale);

    // get which uint to seperate, 8 pixels per uint
    int index = uv.x + uv.y*64; // index is which pixel to grab, not array index or byte index.
    uint value = (uint(pixelData[index / 8]));

    // ascending data looks like this right now:
    // 0x01234567, notice how this makes sense?

    // get which byte of the uint to seperate
    // it moves the byte we need to the right side and masks it out
    // (index / 2): every byte has 2 pixels, so every second index needs the same byte
    // ((index / 2) % 4): amount of bytes to move to the right
    // (((index / 2) % 4) * 8): amount of bits to move to the right
    //  & 0xFFu: only include the right most byte
    value = (value >> (24 - ((index / 2) % 4) * 8)) & 0xFFu;

    // get which half byte to seperate
    // the data in hex looks like 0x01, the first index needs to be pushed to the right by 1 half byte,
    // and the second shouldn't be pushed
    // (1 - index % 2): amount of half bytes to shift
    // ((1 - index % 2) * 4): amount of bits to shift
    // & 0xFu: only include the right most half byte
    value = (value >> ((1 - index % 2) * 4)) & 0xFu;

    // get the palette color for this value
    fragColor = vec4(palette[value], 1.0);
}
)glsl";
