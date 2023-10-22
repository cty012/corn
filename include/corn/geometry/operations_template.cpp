namespace corn {
    template <VectorType Vec>
    float dist(const Vec& v1, const Vec& v2) {
        return (v1 - v2).norm();
    }
}
