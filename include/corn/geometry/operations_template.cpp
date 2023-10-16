namespace corn {
    template <VectorType Vec>
    double dist(const Vec& v1, const Vec& v2) {
        return (v1 - v2).norm();
    }
}
