tag::shader Shader1
{
	Language: "GLSL";
	key Name: "MeshShader1";
	tag::sources Vertex //Реализация GetByTag с where сильно поможет
	{
		Type: "vertex";
		Code: "...code....";
	}
	tag::sources Fragment
	{
		Type: "fragment";
		Code: "...code....";
	}
}
tag::mesh Mesh1
{
	key Name: "Mesh1";
	DecodingFormat: "base64"; //or "text"
	Vertices: "AACAPwAAAEAAAIBCAACARQA=...";		//Vector3
	Normals: "AACAPwAAAEAAAIBCAACARQA=...";			//Vector3
	TextureCoordinates: "AACAPwAAAEAAAIBCAACARQA=...";	//Vector2
	Indices: "AAAAAAEAAgA..."; 
	HasTangents: false;

	Tangents: "AACAPwAAAEAAAIBCAACARQA=...";		//Vector3
	Bitangents: "AACAPwAAAEAAAIBCAACARQA=...";		//Vector3
	
	
        
	
        VertexCount: 1000;
        IndexCount: 3500;
}
tag::material Material1
{
    key Name: "Material1";
    tag::parameters Parameters{
        AlbedoColor: [0.8, 0.7, 0.6];
	EmissionColor: [0.0,0.0,0.0];
        Roughness: 0.6;
	Metallic: 0;
	Emission: 0;
	DoubleSided: false;
	AlphaTest: false;
	AlphaCutOff: 0.5;
    },
    tag::textures Textures{
        Albedo: "textures/mesh_diffuse.jpg";
        Normal: "textures/mesh_normal.jpg";
	Metallic: "textures/mesh_metallic.jpg";
	Roughness: "textures/mesh_roughness.jpg";
	Emission: "textures/mesh_emission.jpg";
	Occlusion: "textures/mesh_occlusion.jpg";
    }
}