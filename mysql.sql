DROP TABLE IF EXISTS graph_roads;
DROP TABLE IF EXISTS graph_buildings;
DROP TABLE IF EXISTS roads;
DROP TABLE IF EXISTS buildings;
DROP TABLE IF EXISTS graphs;

CREATE TABLE buildings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    axis_x INT,
    axis_y INT,
    building_id VARCHAR(255),
    building_kind INT
);

CREATE TABLE roads (
    id INT AUTO_INCREMENT PRIMARY KEY,
    start_building_id INT,
    end_building_id INT,
    crowding DECIMAL(10,6),
    road_distance DECIMAL(10,6),
    road_id VARCHAR(255),
    FOREIGN KEY(start_building_id) REFERENCES buildings(id),
    FOREIGN KEY(end_building_id) REFERENCES buildings(id)
);

CREATE TABLE graphs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    graph_name VARCHAR(255),
    description TEXT
);

CREATE TABLE graph_buildings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    graph_id INT,
    building_id INT,
    FOREIGN KEY(graph_id) REFERENCES graphs(id),
    FOREIGN KEY(building_id) REFERENCES buildings(id)
);

CREATE TABLE graph_roads (
    id INT AUTO_INCREMENT PRIMARY KEY,
    graph_id INT,
    road_id INT,
    FOREIGN KEY(graph_id) REFERENCES graphs(id),
    FOREIGN KEY(road_id) REFERENCES roads(id)
);