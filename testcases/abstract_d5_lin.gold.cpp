// Simple
// system
a = @src[0];
b = @src[1];
c = @src[2];
d = @src[3];
f = ((601 - ((((a * 2) + (b * 13)) + (c * 31)) + (d * 53))) / 73);
@des[0] = ((666 - ((((a * 3) + (b * 17)) + (c * 37)) + (d * 59))) - (f * 79));
@des[1] = ((704 - ((((a * 5) + (b * 19)) + (c * 41)) + (d * 61))) - (f * 83));
@des[2] = ((762 - ((((a * 7) + (b * 23)) + (c * 43)) + (d * 67))) - (f * 89));
@des[3] = ((832 - ((((a * 11) + (b * 29)) + (c * 47)) + (d * 71))) - (f * 97));
