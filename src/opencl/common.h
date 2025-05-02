typedef struct {
    double x;
    double y;
} point_t;

typedef point_t vector_t;

typedef struct {
    point_t pos;
    vector_t vel;

    double radius;
} particle_t;


vector_t create_vector(double x, double y)
{
    vector_t v = {.x = x, .y = y};
    return v;
}

vector_t neg_vec(vector_t vec)
{
    vec.x *= -1;
    vec.y *= -1;
    return vec;
}

vector_t add_vec(vector_t vec1, vector_t vec2)
{
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    return vec1;
}

vector_t sub_vec(vector_t vec1, vector_t vec2)
{
    return add_vec(vec1, neg_vec(vec2));
}

vector_t mult_vec(vector_t vec, double val)
{
    vec.x *= val;
    vec.y *= val;
    return vec;
}

double length_vec(vector_t vec)
{
    return sqrt(vec.x*vec.x + vec.y*vec.y);
}