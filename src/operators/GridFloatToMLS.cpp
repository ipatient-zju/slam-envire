#include <envire/operators/GridFloatToMLS.hpp>

using namespace envire;

ENVIRONMENT_ITEM_DEF( GridFloatToMLS )

GridFloatToMLS::GridFloatToMLS()
    : Operator(1, 1)
{
}

void GridFloatToMLS::unserialize(Serialization& so)
{
    Operator::unserialize(so);
    so.read<std::string>("band_name", band);
}

void GridFloatToMLS::serialize(Serialization& so)
{
    Operator::serialize(so);
    so.write("band_name", band);
}

void GridFloatToMLS::setInput( GridBase* grid, std::string const& band) 
{
    if (!dynamic_cast< Grid<double>* >(grid) && !dynamic_cast< Grid<float>* >(grid))
        throw std::runtime_error("GridFloatToMLS expects either a Grid<float> or a Grid<double>");
    this->band = band;
    Operator::addInput(grid);
}

void GridFloatToMLS::setInput( Grid<float>* grid, std::string const& band) 
{
    setInput(static_cast<GridBase*>(grid), band);
}

void GridFloatToMLS::setInput( Grid<double>* grid, std::string const& band) 
{
    setInput(static_cast<GridBase*>(grid), band);
}

void GridFloatToMLS::setOutput( MLSGrid* mls )
{
    if( env->getOutputs(this).size() > 0 )
        throw std::runtime_error("GridFloatToMLS can only have one output.");
    Operator::addOutput(mls);
}

template<typename T>
static void convert(Grid<T>* grid, std::string const& band_name, MLSGrid* mls)
{
    Transform mls2grid = grid->getEnvironment()->relativeTransform( grid, mls );
    
    boost::multi_array<T, 2>* grid_data;
    if (band_name.empty())
        grid_data = &grid->getGridData();
    else
        grid_data = &grid->getGridData(band_name);

    for (size_t yi = 0; yi < mls->getCellSizeY(); ++yi)
    {
        double y = mls->getScaleY() * yi + mls->getOffsetY();
        for (size_t xi = 0; xi < mls->getCellSizeX(); ++xi)
        {
            double x = mls->getScaleX() * xi + mls->getOffsetX();
            Eigen::Vector3d src_p = mls2grid * Eigen::Vector3d(x, y, 0);
            size_t src_xi, src_yi;
            if (!grid->toGrid(src_p.x(), src_p.y(), src_xi, src_yi))
                continue;

            T value = (*grid_data)[src_yi][src_xi];
            mls->updateCell(xi, yi, value, 0);
        }
    }
}

bool GridFloatToMLS::updateAll()
{
    MLSGrid* mls = static_cast<MLSGrid*>(*env->getOutputs(this).begin());

    Grid<float>* grid = dynamic_cast<Grid<float>*>(*env->getInputs(this).begin());
    if (grid)
        convert(grid, band, mls);
    else
    {
        Grid<double>* grid = dynamic_cast<Grid<double>*>(*env->getInputs(this).begin());
        if (grid)
            convert(grid, band, mls);
        else
            throw std::logic_error("could not find an input of either type Grid<double> or Grid<float>");
    }

    env->itemModified(mls);
    return true;
}
