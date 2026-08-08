/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <gtest/gtest.h>
#include <librepcb/core/fileio/transactionaldirectory.h>
#include <librepcb/core/fileio/transactionalfilesystem.h>
#include <librepcb/core/project/circuit/circuit.h>
#include <librepcb/core/project/circuit/netclass.h>
#include <librepcb/core/project/circuit/netsignal.h>
#include <librepcb/core/project/project.h>
#include <librepcb/core/project/projectloader.h>
#include <librepcb/core/project/schematic/items/si_netline.h>
#include <librepcb/core/project/schematic/items/si_netpoint.h>
#include <librepcb/core/project/schematic/items/si_netsegment.h>
#include <librepcb/core/project/schematic/schematic.h>
#include <librepcb/editor/graphics/graphicslayerlist.h>
#include <librepcb/editor/project/cmd/cmddragselectedschematicitems.h>
#include <librepcb/editor/project/cmd/cmdsimplifyschematicsegments.h>
#include <librepcb/editor/project/projectcrossprobe.h>
#include <librepcb/editor/project/schematic/graphicsitems/sgi_netline.h>
#include <librepcb/editor/project/schematic/schematicgraphicsscene.h>
#include <librepcb/editor/undostack.h>

#include <QtCore>

#include <memory>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace editor {
namespace tests {

/*******************************************************************************
 *  Test Methods
 ******************************************************************************/

TEST(CmdDragSelectedSchematicItemsTest, testMergeCollinearNetLines) {
  // Load an empty project and add a bent net consisting of two lines.
  const FilePath projectFile(TEST_DATA_DIR
                             "/projects/Empty Project/Empty Project.lpp");
  const auto fileSystem =
      TransactionalFileSystem::openRO(projectFile.getParentDir());
  ProjectLoader loader;
  std::unique_ptr<Project> project =
      loader.open(std::make_unique<TransactionalDirectory>(fileSystem),
                  projectFile.getFilename());
  Schematic& schematic = *project->getSchematics().first();
  Circuit& circuit = project->getCircuit();
  auto netSignal = new NetSignal(circuit, Uuid::createRandom(),
                                 *circuit.getNetClasses().first(),
                                 CircuitIdentifier("N"), false);
  circuit.addNetSignal(*netSignal);

  const Uuid segmentUuid = Uuid::createRandom();
  auto segment = new SI_NetSegment(schematic, segmentUuid, *netSignal);
  auto p0 = new SI_NetPoint(*segment, Uuid::createRandom(), Point(0, 2540000));
  auto p1 =
      new SI_NetPoint(*segment, Uuid::createRandom(), Point(2540000, 2540000));
  auto p2 = new SI_NetPoint(*segment, Uuid::createRandom(), Point(5080000, 0));
  auto l0 = new SI_NetLine(*segment, Uuid::createRandom(), *p0, *p1,
                           SI_NetLine::getDefaultWidth());
  auto l1 = new SI_NetLine(*segment, Uuid::createRandom(), *p1, *p2,
                           SI_NetLine::getDefaultWidth());
  segment->addNetPointsAndNetLines({p0, p1, p2}, {l0, l1});
  schematic.addNetSegment(*segment);

  // Select and move the horizontal line down to align it with the other line.
  auto layers = GraphicsLayerList::schematicLayers(nullptr);
  auto crossProbe = std::make_shared<ProjectCrossProbe>();
  bool ignorePlacementLocks = false;
  auto context = std::make_shared<SchematicGraphicsScene::Context>(
      SchematicGraphicsScene::Context{nullptr, crossProbe,
                                      GraphicsLayer::State::Enabled,
                                      ignorePlacementLocks});
  SchematicGraphicsScene scene(schematic, *layers, context);
  scene.getNetLines().value(l0)->setSelected(true);

  UndoStack undoStack;
  auto cmd = new CmdDragSelectedSchematicItems(scene);
  cmd->setCurrentPosition(Point(0, -2540000));
  const QSet<SI_NetSegment*> netSegments = cmd->getModifiedNetSegments();
  const QSet<SI_BusSegment*> busSegments = cmd->getModifiedBusSegments();
  EXPECT_TRUE(undoStack.execCmd(cmd));
  EXPECT_TRUE(undoStack.execCmd(
      new CmdSimplifySchematicSegments(netSegments, busSegments)));

  // The moved lines are collinear and therefore merged into a single line.
  SI_NetSegment* result = schematic.getNetSegments().value(segmentUuid);
  ASSERT_NE(nullptr, result);
  EXPECT_EQ(2, result->getNetPoints().count());
  ASSERT_EQ(1, result->getNetLines().count());
  const SI_NetLine* line = result->getNetLines().first();
  EXPECT_TRUE(((line->getP1().getPosition() == Point(0, 0)) &&
               (line->getP2().getPosition() == Point(5080000, 0))) ||
              ((line->getP1().getPosition() == Point(5080000, 0)) &&
               (line->getP2().getPosition() == Point(0, 0))));

  // Simplification is a separate undo step, restoring the moved split lines.
  undoStack.undo();
  result = schematic.getNetSegments().value(segmentUuid);
  ASSERT_NE(nullptr, result);
  EXPECT_EQ(3, result->getNetPoints().count());
  EXPECT_EQ(2, result->getNetLines().count());
  EXPECT_EQ(Point(0, 0), p0->getPosition());
  EXPECT_EQ(Point(2540000, 0), p1->getPosition());

  // A second undo restores the drag itself.
  undoStack.undo();
  EXPECT_EQ(Point(0, 2540000), p0->getPosition());
  EXPECT_EQ(Point(2540000, 2540000), p1->getPosition());

  undoStack.redo();
  EXPECT_EQ(Point(0, 0), p0->getPosition());
  EXPECT_EQ(Point(2540000, 0), p1->getPosition());

  undoStack.redo();
  result = schematic.getNetSegments().value(segmentUuid);
  ASSERT_NE(nullptr, result);
  EXPECT_EQ(2, result->getNetPoints().count());
  EXPECT_EQ(1, result->getNetLines().count());
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace tests
}  // namespace editor
}  // namespace librepcb
