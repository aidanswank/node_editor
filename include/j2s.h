void to_json(json& j, const Node& n) {
    j = json{{"id", n.id}};
}

void from_json(const json& j, Node& n) {
    j.at("id").get_to(n.id);
}