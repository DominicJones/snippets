// from:
//   http://www.mmartins.me/view/2015/09/27/vector-swizzle-in-d

import std.algorithm;
import std.math;

private enum VecTCoordName = "xyzw";

struct VecT(size_t N, T = float) if ((N > 1) && (N <= VecTCoordName.length)) {
    @property T length() {
        T sqs = data_[0] * data_[0];
        foreach(x; data_[1..$])
            sqs += x * x;
        return sqs.sqrt;
    }

    this(Args...)(Args args) if ((args.length > 0) && (args.length <= N)) {
        static if (args.length == 1) {
            foreach (i; 0..N)
                data_[i] = args[0];
        } else {
            foreach (i, ref arg; args)
                data_[i] = arg;
        }
    }

    @property auto opDispatch(string swizzle)() if ((swizzle.length > 0) && (swizzle.length <= VecTCoordName.length)) {
        import std.string : indexOf, join;
        import std.conv : to;
        import std.array : array;
        
        static if (swizzle.length == 1) {
            enum index = VecTCoordName.indexOf(swizzle[0]);
            return data_[index];
        } else {
            enum indices = swizzle.map!(x => VecTCoordName.indexOf(x)).array;
            enum args = "VecT!(swizzle.length, T)(" ~ indices.map!(x => "data_[" ~ x.to!string ~ "]").join(",") ~ ")";
            return mixin(args);
        }
    }

    private T[N] data_ = cast(T)0;
}

alias Vec2f = VecT!2;
alias Vec3f = VecT!3;
alias Vec4f = VecT!4;

void main() {
    auto v = Vec4f(0.1f, 1.2f, 2.3f, 3.4f);
    auto x = v.x; // returns x coordinate as scalar
    auto a = v.xy;
    auto b = v.zw;
    auto l = v.xyz.length;
    ///auto c = a.z; // error - no z coordinate for 2D vectors
}
