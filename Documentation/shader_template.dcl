tag::shader MeshShader1
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

	tag::uniforms Uniforms
	{

	}
}